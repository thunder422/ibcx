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
#include "functions.h"
#include "operators.h"
#include "programcode.h"
#include "programunit.h"
#include "table.h"


void NoConvert(Compiler &compiler, DataType data_type);

class ExpressionCompilerImpl : public ExpressionCompiler {
public:
    ExpressionCompilerImpl(Compiler &compiler);

    void compileExpression(DataType expected_data_type) override;
    DataType compileExpression() override;

private:
    DataType compileNumExpression(DataType expected_data_type);
    DataType compileImplication();
    DataType compileEquivalence();
    DataType compileOr();
    DataType compileExclusiveOr();
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
    DataType compileFunction();
    DataType addFunctionCode(FunctionCodes *codes, DataType data_type) const;
    DataType compileFunctionArguments(FunctionCodes *codes);
    DataType compileParentheses(DataType expected_data_type);
    DataType compileNumConstant();
    DataType compileOperator(Precedence precedence,
        DataType (ExpressionCompilerImpl::*compile_operand)(),
        OperatorCodes *(*get_codes)(Compiler &, Precedence),
        void (*convert)(Compiler &compiler, DataType data_type) = NoConvert);
    DataType addOperatorCode(OperatorCodes *codes, DataType lhs_data_type, DataType rhs_data_type)
        const;

    Compiler &compiler;
};

// ----------------------------------------

std::unique_ptr<ExpressionCompiler> ExpressionCompiler::create(Compiler &compiler)
{
    return std::unique_ptr<ExpressionCompiler> {new ExpressionCompilerImpl {compiler}};
}

// ----------------------------------------

OperatorCodes *SymbolGetCodes(Compiler &compiler, Precedence precedence)
{
    return compiler.getSymbolOperatorCodes(precedence);
}

OperatorCodes *ComparisonGetCodes(Compiler &compiler, Precedence precedence)
{
    return compiler.getComparisonOperatorCodes(precedence);
}

OperatorCodes *WordGetCodes(Compiler &compiler, Precedence precedence)
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

void ExpressionCompilerImpl::compileExpression(DataType expected_data_type)
{
    auto data_type = compileNumExpression(expected_data_type);
    if (data_type == DataType::Null) {
        throw ExpNumExprError {compiler.getColumn()};
    }
}

DataType ExpressionCompilerImpl::compileExpression()
{
    auto data_type = compileNumExpression(DataType::Null);
    return data_type;
}

DataType ExpressionCompilerImpl::compileNumExpression(DataType expected_data_type)
{
    auto data_type = compileImplication();
    if (data_type != DataType::Null) {
        if (expected_data_type == DataType::Double) {
            ConvertToDouble(compiler, data_type);
            data_type = DataType::Double;
        }
    }
    return data_type;
}

DataType ExpressionCompilerImpl::compileImplication()
{
    return compileOperator(Precedence::Imp, &ExpressionCompilerImpl::compileEquivalence,
        WordGetCodes, ConvertToInteger);
}

DataType ExpressionCompilerImpl::compileEquivalence()
{
    return compileOperator(Precedence::Eqv, &ExpressionCompilerImpl::compileOr, WordGetCodes,
        ConvertToInteger);
}

DataType ExpressionCompilerImpl::compileOr()
{
    return compileOperator(Precedence::Or, &ExpressionCompilerImpl::compileExclusiveOr,
        WordGetCodes, ConvertToInteger);
}

DataType ExpressionCompilerImpl::compileExclusiveOr()
{
    return compileOperator(Precedence::Xor, &ExpressionCompilerImpl::compileAnd, WordGetCodes,
        ConvertToInteger);
}

DataType ExpressionCompilerImpl::compileAnd()
{
    return compileOperator(Precedence::And, &ExpressionCompilerImpl::compileNot, WordGetCodes,
        ConvertToInteger);
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
    return compiler.getWordOperatorCodes(Precedence::Not);
}

DataType ExpressionCompilerImpl::compileNotOperand(OperatorCodes *codes)
{
    auto data_type = compileNot();
    compiler.convertToInteger(data_type);
    return addOperatorCode(codes, DataType::Null, DataType::Null);
}

DataType ExpressionCompilerImpl::compileEquality()
{
    return compileOperator(Precedence::Equality, &ExpressionCompilerImpl::compileRelation,
        ComparisonGetCodes);
}

DataType ExpressionCompilerImpl::compileRelation()
{
    return compileOperator(Precedence::Relation, &ExpressionCompilerImpl::compileSummation,
        ComparisonGetCodes);
}

DataType ExpressionCompilerImpl::compileSummation()
{
    return compileOperator(Precedence::Summation, &ExpressionCompilerImpl::compileModulo,
        SymbolGetCodes);
}

DataType ExpressionCompilerImpl::compileModulo()
{
    return compileOperator(Precedence::Modulo, &ExpressionCompilerImpl::compileIntegerDivision,
        WordGetCodes);
}

DataType ExpressionCompilerImpl::compileIntegerDivision()
{
    return compileOperator(Precedence::IntDivide, &ExpressionCompilerImpl::compileProduct,
        SymbolGetCodes, ConvertToDouble);
}

DataType ExpressionCompilerImpl::compileProduct()
{
    return compileOperator(Precedence::Product, &ExpressionCompilerImpl::compileExponential,
        SymbolGetCodes);
}

DataType ExpressionCompilerImpl::compileExponential()
{
    return compileOperator(Precedence::Exponential, &ExpressionCompilerImpl::compileNumOperand,
        SymbolGetCodes);
}

DataType ExpressionCompilerImpl::compileNegation()
{
    compiler.skipWhiteSpace();
    auto data_type = compileExponential();
    if (data_type == DataType::Null) {
        throw ExpNumExprError {compiler.getColumn()};
    }
    auto codes = Table::operatorCodes(Precedence::Negate);
    compiler.addInstruction(codes->select(data_type).code);
    return data_type;
}

DataType ExpressionCompilerImpl::compileNumOperand()
{
    if (compiler.peekNextChar() == '(') {
        return compileParentheses(DataType::Null);
    }
    if (auto codes = getNotOperatorCodes()) {
        return compileNotOperand(codes);
    }
    auto data_type = compileFunction();
    if (data_type != DataType::Null) {
        return data_type;
    }
    return compileNumConstant();
}

DataType ExpressionCompilerImpl::compileFunction()
{
    if (auto codes = compiler.getNumFunctionCodes()) {
        return compileFunctionArguments(codes);
    }
    return DataType::Null;
}

DataType ExpressionCompilerImpl::compileFunctionArguments(FunctionCodes *codes)
{
    if (compiler.peekNextChar() != '(') {
        throw CompileError {"expected opening parentheses", compiler.getColumn()};
    }
    auto data_type = compileParentheses(codes->argumentDataType());
    return addFunctionCode(codes, data_type);
}

DataType ExpressionCompilerImpl::addFunctionCode(FunctionCodes *codes, DataType data_type) const
{
    auto info = codes->select(data_type);
    compiler.addInstruction(info.code);
    return info.result_data_type;
}

DataType ExpressionCompilerImpl::compileParentheses(DataType expected_data_type)
{
    compiler.getNextChar();
    compiler.skipWhiteSpace();
    auto data_type = compileNumExpression(expected_data_type);
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

DataType ExpressionCompilerImpl::compileOperator(Precedence precedence,
    DataType (ExpressionCompilerImpl::*compile_operand)(),
    OperatorCodes *(*get_codes)(Compiler &compiler, Precedence precedence),
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
            lhs_data_type = addOperatorCode(codes, lhs_data_type, rhs_data_type);
        }
    }
    return lhs_data_type;
}

DataType ExpressionCompilerImpl::addOperatorCode(OperatorCodes *codes, DataType lhs_data_type,
    DataType rhs_data_type) const
{
    auto info = codes->select(lhs_data_type, rhs_data_type);
    compiler.addInstruction(info.code);
    return info.result_data_type;
}
