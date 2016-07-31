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


class ExpressionCompiler::Impl {
public:
    Impl(Compiler &compiler);

    DataType compileExpression(DataType expected_data_type);

private:
    DataType compileNumExpression(DataType expected_data_type);
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
    auto data_type = compileEquality();
    if (expected_data_type != DataType::Null && data_type == DataType::Null) {
        throw ExpNumExprError {compiler.getColumn()};
    }
    return data_type;
}

DataType ExpressionCompiler::Impl::compileEquality()
{
    auto lhs_data_type = compileRelation();
    if (auto codes = compiler.getComparisonOperatorCodes(Precedence::Equality)) {
        auto rhs_data_type = compileRelation();
        auto info = codes->select(lhs_data_type, rhs_data_type);
        compiler.addInstruction(info.code);
    }
    return lhs_data_type;
}

DataType ExpressionCompiler::Impl::compileRelation()
{
    auto lhs_data_type = compileSummation();
    while (auto codes = compiler.getComparisonOperatorCodes(Precedence::Relation)) {
        auto rhs_data_type = compileSummation();
        auto info = codes->select(lhs_data_type, rhs_data_type);
        compiler.addInstruction(info.code);
        lhs_data_type = DataType::Integer;
    }
    return lhs_data_type;
}

DataType ExpressionCompiler::Impl::compileSummation()
{
    auto lhs_data_type = compileModulo();
    while (auto codes = compiler.getSymbolOperatorCodes(Precedence::Summation)) {
        auto rhs_data_type = compileModulo();
        auto info = codes->select(lhs_data_type, rhs_data_type);
        compiler.addInstruction(info.code);
        lhs_data_type = info.result_data_type;
    }
    return lhs_data_type;
}

DataType ExpressionCompiler::Impl::compileModulo()
{
    auto lhs_data_type = compileIntegerDivision();
    while (auto codes = compiler.getWordOperatorCodes(Precedence::Modulo)) {
        auto rhs_data_type = compileIntegerDivision();
        auto info = codes->select(lhs_data_type, rhs_data_type);
        compiler.addInstruction(info.code);
        lhs_data_type = info.result_data_type;
    }
    return lhs_data_type;
}

DataType ExpressionCompiler::Impl::compileIntegerDivision()
{
    auto lhs_data_type = compileProduct();
    while (auto codes = compiler.getSymbolOperatorCodes(Precedence::IntDivide)) {
        compiler.convertToDouble(lhs_data_type);
        auto rhs_data_type = compileProduct();
        compiler.convertToDouble(rhs_data_type);
        auto info = codes->select(lhs_data_type, rhs_data_type);
        compiler.addInstruction(info.code);
        lhs_data_type = info.result_data_type;
    }
    return lhs_data_type;
}

DataType ExpressionCompiler::Impl::compileProduct()
{
    auto lhs_data_type = compileExponential();
    while (auto codes = compiler.getSymbolOperatorCodes(Precedence::Product)) {
        auto rhs_data_type = compileExponential();
        auto info = codes->select(lhs_data_type, rhs_data_type);
        compiler.addInstruction(info.code);
        lhs_data_type = info.result_data_type;
    }
    return lhs_data_type;
}

DataType ExpressionCompiler::Impl::compileExponential()
{
    auto lhs_data_type = compileNumOperand();
    if (lhs_data_type != DataType::Null) {
        while (auto codes = compiler.getSymbolOperatorCodes(Precedence::Exponential)) {
            auto rhs_data_type = compileNumOperand();
            if (rhs_data_type == DataType::Null) {
                throw ExpNumExprError {compiler.getColumn()};
            }
            auto info = codes->select(lhs_data_type, rhs_data_type);
            compiler.addInstruction(info.code);
            lhs_data_type = info.result_data_type;
        }
    }
    return lhs_data_type;
}

DataType ExpressionCompiler::Impl::compileNegation()
{
    compiler.skipWhiteSpace();
    auto data_type = compileExponential();
    if (data_type == DataType::Null) {
        throw ExpNumExprError {compiler.getColumn()};
    }
    auto codes = Precedence::operatorCodes(Precedence::Negate);
    compiler.addInstruction(codes->select(data_type).code);
    return data_type;
}

DataType ExpressionCompiler::Impl::compileNumOperand()
{
    if (compiler.peekNextChar() == '(') {
        return compileParentheses();
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
