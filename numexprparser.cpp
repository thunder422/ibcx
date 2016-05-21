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


NumExprParser::NumExprParser(std::istream &is) :
    is {is}
{

}

DataType NumExprParser::parse(ProgramCode &code_line, ProgramUnit &program)
{
    (void)code_line;
    (void)program;
    return DataType::Null;
}
