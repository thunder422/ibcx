/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>

#include "numexprparser.h"
#include "programcode.h"
#include "programunit.h"


class NumExprParser::Impl {
public:
    Impl(std::istream &is, ProgramCode &code_line, ProgramUnit &program);

    DataType parseOperand();

private:
    std::istream &is;
    ProgramCode code_line;
    ProgramUnit program;
};

NumExprParser::NumExprParser(std::istream &is, ProgramCode &code_line, ProgramUnit &program) :
    impl {new Impl(is, code_line, program)}
{
}

NumExprParser::~NumExprParser()
{
}

DataType NumExprParser::parse()
{
    return impl->parseOperand();
}

NumExprParser::Impl::Impl(std::istream &is, ProgramCode &code_line, ProgramUnit &program) :
    is {is},
    code_line {code_line},
    program {program}
{
}



DataType NumExprParser::Impl::parseOperand()
{
    return DataType::Integer;
}
