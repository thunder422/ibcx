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

    void compile();

private:
    std::string parseKeyword();

    Compiler &compiler;
};

// ----------------------------------------

CommandCompiler::CommandCompiler(Compiler &compiler) :
    pimpl {new Impl(compiler)}
{
}

void CommandCompiler::operator()()
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

void CommandCompiler::Impl::compile()
{
    if (compiler.is.peek() == EOF) {
        return;
    }
    auto keyword = parseKeyword();
    if (keyword.empty()) {
        throw CompileError{"expected command keyword", 0};
    }
    auto code = CommandCode::find(keyword);
    code->compile(compiler);
}

std::string CommandCompiler::Impl::parseKeyword()
{
    std::string keyword;
    compiler.is >> std::ws;
    while (isalpha(compiler.is.peek())) {
        keyword += compiler.is.get();
    }
    compiler.is >> std::ws;
    return keyword;
}
