/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>

#include "compileerror.h"
#include "compiler.h"
#include "constnumcompiler.h"
#include "expressioncompiler.h"
#include "operators.h"
#include "precedence.h"
#include "programcode.h"
#include "programunit.h"


void NoConvert(Compiler &compiler, DataType data_type);

class ExpressionCompilerImpl : public ExpressionCompiler {
public:
    ExpressionCompilerImpl(Compiler &compiler);

    DataType compile(DataType expected_data_type) override;

private:
    DataType compileNumExpression(DataType expected_data_type);
    DataType compileImp();
    DataType compileEqv();
    DataType compileOr();
    DataType compileXor();
    DataType compileAnd();
    DataType compileNot();
    OperatorCodes *getNotOperatorCodes();
    DataType compileNotOperand(OperatorCodes *codes);
    DataType compileEquality();
    DataType compileRelation();
    DataType compileSummation();
    DataType compileModulo();
    DataType compileIntegerDivision();
    DataType compileProduct();
    DataType compileExponential();
    DataType compileNegation();
    DataType compileNumOperand();
    DataType compileParentheses();
    DataType compileNumConstant();
    DataType compileOperator(Precedence::Level precedence,
        DataType (ExpressionCompilerImpl::*compile_operand)(),
        OperatorCodes *(*get_codes)(Compiler &compiler, Precedence::Level precedence),
        void (*convert)(Compiler &compiler, DataType data_type) = NoConvert);
    DataType addSelectedCode(OperatorCodes *codes, DataType lhs_data_type, DataType rhs_data_type)
        const;

    Compiler &compiler;
};

// ----------------------------------------

std::unique_ptr<ExpressionCompiler> ExpressionCompiler::create(Compiler &compiler)
{
    return std::unique_ptr<ExpressionCompiler> {new ExpressionCompilerImpl {compiler}};
}

// ----------------------------------------

OperatorCodes *SymbolGetCodes(Compiler &compiler, Precedence::Level precedence)
{
    return compiler.getSymbolOperatorCodes(precedence);
}

OperatorCodes *ComparisonGetCodes(Compiler &compiler, Precedence::Level precedence)
{
    return compiler.getComparisonOperatorCodes(precedence);
}

OperatorCodes *WordGetCodes(Compiler &compiler, Precedence::Level precedence)
{
    return compiler.getWordOperatorCodes(precedence);
}

void NoConvert(Compiler &compiler, DataType data_type)
{
    (void)compiler;
    (void)data_type;
}

void ConvertToDouble(Compiler &compiler, DataType data_type)
{
    compiler.convertToDouble(data_type);
}

void ConvertToInteger(Compiler &compiler, DataType data_type)
{
    compiler.convertToInteger(data_type);
}

// ----------------------------------------

ExpressionCompilerImpl::ExpressionCompilerImpl(Compiler &compiler) :
    compiler {compiler}
{
}

DataType ExpressionCompilerImpl::compile(DataType expected_data_type)
{
    return compileNumExpression(expected_data_type);
}

DataType ExpressionCompilerImpl::compileNumExpression(DataType expected_data_type)
{
    auto data_type = compileImp();
    if (expected_data_type != DataType::Null && data_type == DataType::Null) {
        throw ExpNumExprError {compiler.getColumn()};
    }
    return data_type;
}

DataType ExpressionCompilerImpl::compileImp()
{
    return compileOperator(Precedence::Level::Imp, &ExpressionCompilerImpl::compileEqv,
        WordGetCodes, ConvertToInteger);
}

DataType ExpressionCompilerImpl::compileEqv()
{
    return compileOperator(Precedence::Level::Eqv, &ExpressionCompilerImpl::compileOr, WordGetCodes,
        ConvertToInteger);
}

DataType ExpressionCompilerImpl::compileOr()
{
    return compileOperator(Precedence::Level::Or, &ExpressionCompilerImpl::compileXor, WordGetCodes,
        ConvertToInteger);
}

DataType ExpressionCompilerImpl::compileXor()
{
    return compileOperator(Precedence::Level::Xor, &ExpressionCompilerImpl::compileAnd,
        WordGetCodes, ConvertToInteger);
}

DataType ExpressionCompilerImpl::compileAnd()
{
    return compileOperator(Precedence::Level::And, &ExpressionCompilerImpl::compileNot,
        WordGetCodes, ConvertToInteger);
}

DataType ExpressionCompilerImpl::compileNot()
{
    if (auto codes = getNotOperatorCodes()) {
        return compileNotOperand(codes);
    } else {
        return compileEquality();
    }
}

OperatorCodes *ExpressionCompilerImpl::getNotOperatorCodes()
{
    return compiler.getWordOperatorCodes(Precedence::Level::Not);
}

DataType ExpressionCompilerImpl::compileNotOperand(OperatorCodes *codes)
{
    auto data_type = compileNot();
    compiler.convertToInteger(data_type);
    return addSelectedCode(codes, DataType::Null, DataType::Null);
}

DataType ExpressionCompilerImpl::compileEquality()
{
    return compileOperator(Precedence::Level::Equality, &ExpressionCompilerImpl::compileRelation,
        ComparisonGetCodes);
}

DataType ExpressionCompilerImpl::compileRelation()
{
    return compileOperator(Precedence::Level::Relation, &ExpressionCompilerImpl::compileSummation,
        ComparisonGetCodes);
}

DataType ExpressionCompilerImpl::compileSummation()
{
    return compileOperator(Precedence::Level::Summation, &ExpressionCompilerImpl::compileModulo,
        SymbolGetCodes);
}

DataType ExpressionCompilerImpl::compileModulo()
{
    return compileOperator(Precedence::Level::Modulo,
        &ExpressionCompilerImpl::compileIntegerDivision, WordGetCodes);
}

DataType ExpressionCompilerImpl::compileIntegerDivision()
{
    return compileOperator(Precedence::Level::IntDivide, &ExpressionCompilerImpl::compileProduct,
        SymbolGetCodes, ConvertToDouble);
}

DataType ExpressionCompilerImpl::compileProduct()
{
    return compileOperator(Precedence::Level::Product, &ExpressionCompilerImpl::compileExponential,
        SymbolGetCodes);
}

DataType ExpressionCompilerImpl::compileExponential()
{
    return compileOperator(Precedence::Level::Exponential,
        &ExpressionCompilerImpl::compileNumOperand, SymbolGetCodes);
}

DataType ExpressionCompilerImpl::compileNegation()
{
    compiler.skipWhiteSpace();
    auto data_type = compileExponential();
    if (data_type == DataType::Null) {
        throw ExpNumExprError {compiler.getColumn()};
    }
    auto codes = Precedence::operatorCodes(Precedence::Level::Negate);
    compiler.addInstruction(codes->select(data_type).code);
    return data_type;
}

DataType ExpressionCompilerImpl::compileNumOperand()
{
    if (compiler.peekNextChar() == '(') {
        return compileParentheses();
    }
    if (auto codes = getNotOperatorCodes()) {
        return compileNotOperand(codes);
    }
    return compileNumConstant();
}

DataType ExpressionCompilerImpl::compileParentheses()
{
    compiler.getNextChar();
    compiler.skipWhiteSpace();
    auto data_type = compileNumExpression(DataType::Double);
    if (compiler.peekNextChar() != ')') {
        throw CompileError {"expected closing parentheses", compiler.getColumn()};
    }
    compiler.getNextChar();
    return data_type;
}

DataType ExpressionCompilerImpl::compileNumConstant()
{
    auto constant_number = ConstNumCompiler::create(compiler);
    auto data_type = constant_number->compile();
    if (data_type == DataType::Null && constant_number->negateOperator()) {
        return compileNegation();
    }
    if (auto unparsed_char = constant_number->unparsedChar()) {
        compiler.parseKeyword(unparsed_char);
    }
    return data_type;
}

DataType ExpressionCompilerImpl::compileOperator(Precedence::Level precedence,
    DataType (ExpressionCompilerImpl::*compile_operand)(),
    OperatorCodes *(*get_codes)(Compiler &compiler, Precedence::Level precedence),
    void (*convert)(Compiler &compiler, DataType data_type))
{
    auto lhs_data_type = (this->*compile_operand)();
    if (lhs_data_type != DataType::Null) {
        while (auto codes = get_codes(compiler, precedence)) {
            convert(compiler, lhs_data_type);
            auto rhs_data_type = (this->*compile_operand)();
            if (rhs_data_type == DataType::Null) {
                throw ExpNumExprError {compiler.getColumn()};
            }
            convert(compiler, rhs_data_type);
            lhs_data_type = addSelectedCode(codes, lhs_data_type, rhs_data_type);
        }
    }
    return lhs_data_type;
}

DataType ExpressionCompilerImpl::addSelectedCode(OperatorCodes *codes, DataType lhs_data_type,
    DataType rhs_data_type) const
{
    auto info = codes->select(lhs_data_type, rhs_data_type);
    compiler.addInstruction(info.code);
    return info.result_data_type;
}
