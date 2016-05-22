/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "numexprparser.h"
#include "parseerror.h"
#include "programcode.h"
#include "programunit.h"
#include "support.h"


TEST_CASE("parse expressions with constants", "[constant]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("parse an expression with an integer constant to the code line")
    {
        extern Code constIntCode;

        std::istringstream iss {"1"};
        auto data_type = NumExprParser{iss, code_line, program}.parse();
        REQUIRE_INTEGER_OPERAND("1");
    }
    SECTION("verify an error is thrown when nothing is in the input stream")
    {
        std::istringstream iss;
        NumExprParser expression {iss, code_line, program};
        REQUIRE_THROWS_AS(expression.parse(), ParseError);
    }
}
