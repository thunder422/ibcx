/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "commandcode.h"
#include "programcode.h"


class EndCode : public CommandCode {
public:
    EndCode();
    void parse(std::istream &is, ProgramCode &code_line, ProgramUnit &program) override;
};

EndCode end_code;

EndCode::EndCode() :
    CommandCode("END")
{
}

void EndCode::parse(std::istream &is, ProgramCode &code_line, ProgramUnit &program)
{
    (void)is;
    (void)program;
    code_line.emplace_back(end_code);
}
