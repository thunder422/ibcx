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
    DataType compileAnd();
    DataType compileLogicOperator(Precedence::Level precedence, DataType (Impl::*compile_next)());
    DataType compileNot();
    DataType compileEquality();
    DataType compileRelation();
    DataType compileComparisonOperator(Precedence::Level precedence,
        DataType (Impl::*compile_next)());
    DataType compileSummation();
    DataType compileModulo();
    DataType compileIntegerDivision();
    DataType compileProduct();
    DataType compileExponential();
    DataType compileMathOperator(Precedence::Level precedence, DataType (Impl::*compile_next)());
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
    auto data_type = compileAnd();
    if (expected_data_type != DataType::Null && data_type == DataType::Null) {
        throw ExpNumExprError {compiler.getColumn()};
    }
    return data_type;
}

DataType ExpressionCompiler::Impl::compileAnd()
{
    return compileLogicOperator(Precedence::And, &Impl::compileNot);
}

DataType ExpressionCompiler::Impl::compileLogicOperator(Precedence::Level precedence,
    DataType (Impl::*compile_next)())
{
    auto data_type = (this->*compile_next)();
    while (auto codes = compiler.getWordOperatorCodes(precedence)) {
        compiler.convertToInteger(data_type);
        data_type = (this->*compile_next)();
        compiler.convertToInteger(data_type);
        auto info = codes->select(DataType::Null, DataType::Null);
        compiler.addInstruction(info.code);
        data_type = DataType::Integer;
    }
    return data_type;
}

DataType ExpressionCompiler::Impl::compileNot()
{
    if (auto codes = compiler.getWordOperatorCodes(Precedence::Not)) {
        auto data_type = compileNot();
        compiler.convertToInteger(data_type);
        auto info = codes->select(DataType::Null, DataType::Null);
        compiler.addInstruction(info.code);
        return DataType::Integer;
    } else {
        return compileEquality();
    }
}

DataType ExpressionCompiler::Impl::compileEquality()
{
    return compileComparisonOperator(Precedence::Equality, &Impl::compileRelation);
}

DataType ExpressionCompiler::Impl::compileRelation()
{
    return compileComparisonOperator(Precedence::Relation, &Impl::compileSummation);
}

DataType ExpressionCompiler::Impl::compileComparisonOperator(Precedence::Level precedence,
    DataType (Impl::*compile_next)())
{
    auto lhs_data_type = (this->*compile_next)();
    if (lhs_data_type != DataType::Null) {
        while (auto codes = compiler.getComparisonOperatorCodes(precedence)) {
            auto rhs_data_type = (this->*compile_next)();
            if (rhs_data_type == DataType::Null) {
                throw ExpNumExprError {compiler.getColumn()};
            }
            auto info = codes->select(lhs_data_type, rhs_data_type);
            compiler.addInstruction(info.code);
            lhs_data_type = DataType::Integer;
        }
    }
    return lhs_data_type;
}

DataType ExpressionCompiler::Impl::compileSummation()
{
    return compileMathOperator(Precedence::Summation, &Impl::compileModulo);
}

DataType ExpressionCompiler::Impl::compileModulo()
{
    auto lhs_data_type = compileIntegerDivision();
    if (lhs_data_type != DataType::Null) {
        while (auto codes = compiler.getWordOperatorCodes(Precedence::Modulo)) {
            auto rhs_data_type = compileIntegerDivision();
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

DataType ExpressionCompiler::Impl::compileIntegerDivision()
{
    auto lhs_data_type = compileProduct();
    if (lhs_data_type != DataType::Null) {
        while (auto codes = compiler.getSymbolOperatorCodes(Precedence::IntDivide)) {
            compiler.convertToDouble(lhs_data_type);
            auto rhs_data_type = compileProduct();
            if (rhs_data_type == DataType::Null) {
                throw ExpNumExprError {compiler.getColumn()};
            }
            compiler.convertToDouble(rhs_data_type);
            auto info = codes->select(lhs_data_type, rhs_data_type);
            compiler.addInstruction(info.code);
            lhs_data_type = info.result_data_type;
        }
    }
    return lhs_data_type;
}

DataType ExpressionCompiler::Impl::compileProduct()
{
    return compileMathOperator(Precedence::Product, &Impl::compileExponential);
}

DataType ExpressionCompiler::Impl::compileExponential()
{
    return compileMathOperator(Precedence::Exponential, &Impl::compileNumOperand);
}

DataType ExpressionCompiler::Impl::compileMathOperator(Precedence::Level precedence,
    DataType (Impl::*compile_next)())
{
    auto lhs_data_type = (this->*compile_next)();
    if (lhs_data_type != DataType::Null) {
        while (auto codes = compiler.getSymbolOperatorCodes(precedence)) {
            auto rhs_data_type = (this->*compile_next)();
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
