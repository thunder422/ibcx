/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>

#include "commandcode.h"
#include "expressionparser.h"
#include "programcode.h"


class PrintCode : public CommandCode {
public:
    PrintCode();
    void parse(std::istream &is, ProgramCode &code_line, ProgramUnit &program) override;
};

PrintCode print_code;
Code print_int_code;

PrintCode::PrintCode() :
    CommandCode("PRINT")
{
}

void PrintCode::parse(std::istream &is, ProgramCode &code_line, ProgramUnit &program)
{
    if (is.peek() != EOF) {
        ExpressionParser{is, code_line, program}(DataType::Null);
        code_line.emplace_back(print_int_code);
    }
    code_line.emplace_back(print_code);
}
