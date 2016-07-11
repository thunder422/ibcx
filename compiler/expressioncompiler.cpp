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
#include "programcode.h"
#include "programunit.h"


class ExpressionCompiler::Impl {
public:
    Impl(Compiler &compiler);

    DataType compileExpression(DataType expected_data_type);

private:
    DataType compileNumExpression(DataType expected_data_type);
    DataType compileProduct();
    DataType compileExponential();
    bool isOperatorChar(char operator_char);
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
    auto data_type = compileProduct();
    if (expected_data_type != DataType::Null && data_type == DataType::Null) {
        throw ExpNumExprError {compiler.getColumn()};
    }
    return data_type;
}

DataType ExpressionCompiler::Impl::compileProduct()
{
    extern NumOperatorCodes mul_codes;

    auto lhs_data_type = compileExponential();
    if (isOperatorChar('*')) {
        auto rhs_data_type = compileExponential();
        auto info = mul_codes.select(lhs_data_type, rhs_data_type);
        compiler.addInstruction(info.code);
        lhs_data_type = info.result_data_type;
    }
    return lhs_data_type;
}

DataType ExpressionCompiler::Impl::compileExponential()
{
    extern NumOperatorCodes exp_codes;

    auto lhs_data_type = compileNumOperand();
    if (lhs_data_type != DataType::Null) {
        while (isOperatorChar('^')) {
            auto rhs_data_type = compileNumOperand();
            if (rhs_data_type == DataType::Null) {
                throw ExpNumExprError {compiler.getColumn()};
            }
            auto info = exp_codes.select(lhs_data_type, rhs_data_type);
            compiler.addInstruction(info.code);
            lhs_data_type = info.result_data_type;
        }
    }
    return lhs_data_type;
}

bool ExpressionCompiler::Impl::isOperatorChar(char operator_char)
{
    compiler.skipWhiteSpace();
    if (compiler.peekNextChar() == operator_char) {
        compiler.getNextChar();
        compiler.skipWhiteSpace();
        return true;
    }
    return false;
}

DataType ExpressionCompiler::Impl::compileNegation()
{
    extern UnaryOperatorCodes neg_codes;

    compiler.skipWhiteSpace();
    auto data_type = compileExponential();
    if (data_type == DataType::Null) {
        throw ExpNumExprError {compiler.getColumn()};
    }
    compiler.addInstruction(neg_codes.select(data_type));
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
