/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>

#include "commandcode.h"
#include "commandparser.h"
#include "compiler.h"
#include "parseerror.h"
#include "programcode.h"


class CommandParser::Impl {
public:
    Impl(Compiler &compiler);

    void parse();

private:
    std::string parseKeyword();

    Compiler &compiler;
};

// ----------------------------------------

CommandParser::CommandParser(Compiler &compiler) :
    pimpl {new Impl(compiler)}
{
}

void CommandParser::operator()()
{
    return pimpl->parse();
}

CommandParser::~CommandParser()
{
}

// ----------------------------------------

CommandParser::Impl::Impl(Compiler &compiler) :
    compiler {compiler}
{
}

void CommandParser::Impl::parse()
{
    if (compiler.is.peek() == EOF) {
        return;
    }
    auto keyword = parseKeyword();
    if (keyword.empty()) {
        throw ParseError{"expected command keyword", 0};
    }
    auto code = CommandCode::find(keyword);
    code->compile(compiler);
}

std::string CommandParser::Impl::parseKeyword()
{
    std::string keyword;
    compiler.is >> std::ws;
    while (isalpha(compiler.is.peek())) {
        keyword += compiler.is.get();
    }
    compiler.is >> std::ws;
    return keyword;
}
