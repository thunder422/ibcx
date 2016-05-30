/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>

#include "commandcode.h"
#include "commandcompiler.h"
#include "compiler.h"
#include "compileerror.h"
#include "programcode.h"


class CommandCompiler::Impl {
public:
    Impl(Compiler &compiler);

    ProgramCode &&compile();

private:
    Compiler &compiler;
};

// ----------------------------------------

CommandCompiler::CommandCompiler(Compiler &compiler) :
    pimpl {new Impl(compiler)}
{
}

ProgramCode &&CommandCompiler::operator()()
{
    return pimpl->compile();
}

CommandCompiler::~CommandCompiler()
{
}

// ----------------------------------------

CommandCompiler::Impl::Impl(Compiler &compiler) :
    compiler {compiler}
{
}

ProgramCode &&CommandCompiler::Impl::compile()
{
    if (compiler.peekNextChar() != EOF) {
        auto keyword = compiler.getKeyword();
        if (keyword.empty()) {
            throw CompileError{"expected command keyword", 0};
        }
        auto code = CommandCode::find(keyword);
        code->compile(compiler);
    }
    return compiler.getCodeLine();
}
