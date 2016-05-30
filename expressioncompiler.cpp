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


class ExpressionCompiler::Impl {
public:
    Impl(Compiler &compiler);

    DataType compileExpression(DataType expected_data_type);

private:
    DataType compileNumOperand();

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
    (void)expected_data_type;
    return compileNumOperand();
}

DataType ExpressionCompiler::Impl::compileNumOperand()
{
    ConstNumCompiler compile_constant {compiler};
    unsigned column = compiler.getColumn();
    auto data_type = compile_constant();
    if (data_type == DataType::Null) {
        throw CompileError {"expected numeric expression", column};
    }
    return data_type;
}
