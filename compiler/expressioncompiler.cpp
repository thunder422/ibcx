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

class ExpressionCompiler::Impl {
public:
    Impl(Compiler &compiler);

    DataType compileExpression(DataType expected_data_type);

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
    DataType compileOperator(Precedence::Level precedence, DataType (Impl::*compile_operand)(),
        OperatorCodes *(*get_codes)(Compiler &compiler, Precedence::Level precedence),
        void (*convert)(Compiler &compiler, DataType data_type) = NoConvert);
    DataType addSelectedCode(OperatorCodes *codes, DataType lhs_data_type, DataType rhs_data_type)
        const;

    Compiler &compiler;
};

ExpressionCompiler::ExpressionCompiler(Compiler &compiler) :
    pimpl {new Impl(compiler)}
{
}

DataType ExpressionCompiler::operator()(DataType expected_data_type)
{
    return pimpl->compileExpression(expected_data_type);
}

ExpressionCompiler::~ExpressionCompiler()
{
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

ExpressionCompiler::Impl::Impl(Compiler &compiler) :
    compiler {compiler}
{
}

DataType ExpressionCompiler::Impl::compileExpression(DataType expected_data_type)
{
    return compileNumExpression(expected_data_type);
}

DataType ExpressionCompiler::Impl::compileNumExpression(DataType expected_data_type)
{
    auto data_type = compileImp();
    if (expected_data_type != DataType::Null && data_type == DataType::Null) {
        throw ExpNumExprError {compiler.getColumn()};
    }
    return data_type;
}

DataType ExpressionCompiler::Impl::compileImp()
{
    return compileOperator(Precedence::Level::Imp, &Impl::compileEqv, WordGetCodes,
        ConvertToInteger);
}

DataType ExpressionCompiler::Impl::compileEqv()
{
    return compileOperator(Precedence::Level::Eqv, &Impl::compileOr, WordGetCodes,
        ConvertToInteger);
}

DataType ExpressionCompiler::Impl::compileOr()
{
    return compileOperator(Precedence::Level::Or, &Impl::compileXor, WordGetCodes,
        ConvertToInteger);
}

DataType ExpressionCompiler::Impl::compileXor()
{
    return compileOperator(Precedence::Level::Xor, &Impl::compileAnd, WordGetCodes,
        ConvertToInteger);
}

DataType ExpressionCompiler::Impl::compileAnd()
{
    return compileOperator(Precedence::Level::And, &Impl::compileNot, WordGetCodes,
        ConvertToInteger);
}

DataType ExpressionCompiler::Impl::compileNot()
{
    if (auto codes = getNotOperatorCodes()) {
        return compileNotOperand(codes);
    } else {
        return compileEquality();
    }
}

OperatorCodes *ExpressionCompiler::Impl::getNotOperatorCodes()
{
    return compiler.getWordOperatorCodes(Precedence::Level::Not);
}

DataType ExpressionCompiler::Impl::compileNotOperand(OperatorCodes *codes)
{
    auto data_type = compileNot();
    compiler.convertToInteger(data_type);
    return addSelectedCode(codes, DataType::Null, DataType::Null);
}

DataType ExpressionCompiler::Impl::compileEquality()
{
    return compileOperator(Precedence::Level::Equality, &Impl::compileRelation, ComparisonGetCodes);
}

DataType ExpressionCompiler::Impl::compileRelation()
{
    return compileOperator(Precedence::Level::Relation, &Impl::compileSummation,
        ComparisonGetCodes);
}

DataType ExpressionCompiler::Impl::compileSummation()
{
    return compileOperator(Precedence::Level::Summation, &Impl::compileModulo, SymbolGetCodes);
}

DataType ExpressionCompiler::Impl::compileModulo()
{
    return compileOperator(Precedence::Level::Modulo, &Impl::compileIntegerDivision, WordGetCodes);
}

DataType ExpressionCompiler::Impl::compileIntegerDivision()
{
    return compileOperator(Precedence::Level::IntDivide, &Impl::compileProduct, SymbolGetCodes,
        ConvertToDouble);
}

DataType ExpressionCompiler::Impl::compileProduct()
{
    return compileOperator(Precedence::Level::Product, &Impl::compileExponential, SymbolGetCodes);
}

DataType ExpressionCompiler::Impl::compileExponential()
{
    return compileOperator(Precedence::Level::Exponential, &Impl::compileNumOperand,
        SymbolGetCodes);
}

DataType ExpressionCompiler::Impl::compileNegation()
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

DataType ExpressionCompiler::Impl::compileNumOperand()
{
    if (compiler.peekNextChar() == '(') {
        return compileParentheses();
    }
    if (auto codes = getNotOperatorCodes()) {
        return compileNotOperand(codes);
    }
    return compileNumConstant();
}

DataType ExpressionCompiler::Impl::compileParentheses()
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

DataType ExpressionCompiler::Impl::compileNumConstant()
{
    ConstNumCompiler compile_constant {compiler};
    auto data_type = compile_constant();
    if (data_type == DataType::Null && compile_constant.negateOperator()) {
        return compileNegation();
    }
    return data_type;
}

DataType ExpressionCompiler::Impl::compileOperator(Precedence::Level precedence,
    DataType (Impl::*compile_operand)(),
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

DataType ExpressionCompiler::Impl::addSelectedCode(OperatorCodes *codes, DataType lhs_data_type,
    DataType rhs_data_type) const
{
    auto info = codes->select(lhs_data_type, rhs_data_type);
    compiler.addInstruction(info.code);
    return info.result_data_type;
}
