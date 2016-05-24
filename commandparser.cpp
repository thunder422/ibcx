/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>

#include "commandcode.h"
#include "commandparser.h"
#include "parseerror.h"
#include "programcode.h"


class CommandParser::Impl {
public:
    Impl(std::istream &is, ProgramCode &code_line, ProgramUnit &program);

    void parse();

private:
    std::string parseKeyword();

    std::istream &is;
    ProgramCode &code_line;
    ProgramUnit &program;
};

// ----------------------------------------

CommandParser::CommandParser(std::istream &is, ProgramCode &code_line, ProgramUnit &program) :
    pimpl {new Impl(is, code_line, program)}
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

CommandParser::Impl::Impl(std::istream &is, ProgramCode &code_line, ProgramUnit &program) :
    is {is},
    code_line {code_line},
    program {program}
{
}

void CommandParser::Impl::parse()
{
    if (is.peek() == EOF) {
        return;
    }
    auto keyword = parseKeyword();
    auto code = CommandCode::find(keyword);
    code->parse(is, code_line, program);
}

std::string CommandParser::Impl::parseKeyword()
{
    std::string keyword;
    while (isalpha(is.peek())) {
        keyword += is.get();
    }
    return keyword;
}
