/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>

#include "compiler.h"
#include "constnumcompiler.h"
#include "expressioncompiler.h"
#include "compileerror.h"
#include "programcode.h"
#include "programunit.h"


struct OperatorInfo {
    Code &code;
    DataType result_data_type;
};


class ExpressionCompiler::Impl {
public:
    Impl(Compiler &compiler);

    DataType compileExpression(DataType expected_data_type);

private:
    DataType compileNumExpression(DataType expected_data_type);
    DataType compileExponential();
    bool isOperatorChar(char operator_char);
    OperatorInfo selectExpCode(DataType lhs_data_type, DataType rhs_data_type);
    DataType compileNegation();
    DataType compileNumOperand();

    Compiler &compiler;
    int column;
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
    auto data_type = compileExponential();
    if (expected_data_type != DataType::Null && data_type == DataType::Null) {
        throw ExpNumExprError {column};
    }
    return data_type;
}

DataType ExpressionCompiler::Impl::compileExponential()
{
    auto lhs_data_type = compileNumOperand();
    if (lhs_data_type != DataType::Null) {
        if (isOperatorChar('^')) {
            auto rhs_data_type = compileNumOperand();
            if (rhs_data_type == DataType::Null) {
                throw ExpNumExprError {compiler.getColumn()};
            }
            auto info = selectExpCode(lhs_data_type, rhs_data_type);
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

Code exp_dbl_code {nullptr, nullptr};
Code exp_lhs_int_code {nullptr, nullptr};
Code exp_rhs_int_code {nullptr, nullptr};
Code exp_int_code {nullptr, nullptr};

OperatorInfo ExpressionCompiler::Impl::selectExpCode(DataType lhs_data_type, DataType rhs_data_type)
{
    if (lhs_data_type == DataType::Double) {
        return rhs_data_type == DataType::Double
            ? OperatorInfo {exp_dbl_code, DataType::Double}
            : OperatorInfo {exp_rhs_int_code, DataType::Double};
    } else {
        return rhs_data_type == DataType::Double
            ? OperatorInfo {exp_lhs_int_code, DataType::Double}
            : OperatorInfo {exp_int_code, DataType::Integer};
    }
}

DataType ExpressionCompiler::Impl::compileNegation()
{
    extern Code neg_dbl_code;
    extern Code neg_int_code;

    compiler.skipWhiteSpace();
    column = compiler.getColumn();
    auto data_type = compileNumOperand();
    if (data_type == DataType::Null) {
        throw ExpNumExprError {column};
    }
    compiler.addInstruction(data_type == DataType::Double ? neg_dbl_code : neg_int_code);
    return data_type;
}

DataType ExpressionCompiler::Impl::compileNumOperand()
{
    ConstNumCompiler compile_constant {compiler};
    column = compiler.getColumn();
    auto data_type = compile_constant();
    if (data_type == DataType::Null && compile_constant.negateOperator()) {
        return compileNegation();
    }
    return data_type;
}
