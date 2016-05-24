/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "commandcode.h"
#include "commandparser.h"
#include "parseerror.h"
#include "programcode.h"
#include "programunit.h"


TEST_CASE("parse simple commands", "[simple]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("parse an empty command line")
    {
        std::istringstream iss {""};
        CommandParser parse_command {iss, code_line, program};
        parse_command();
        REQUIRE(code_line.empty());
    }
    SECTION("parse a simple PRINT command")
    {
        std::istringstream iss {"PRINT"};
        CommandParser parse_command {iss, code_line, program};
        parse_command();
        REQUIRE(code_line.size() == 1);
        auto code = CommandCode::find("PRINT");
        REQUIRE(code != nullptr);
        REQUIRE(code_line[0].instructionCode() == code->getValue());
    }
    SECTION("parse an END command")
    {
        std::istringstream iss {"END"};
        CommandParser parse_command {iss, code_line, program};
        parse_command();
        REQUIRE(code_line.size() == 1);
        auto code = CommandCode::find("END");
        REQUIRE(code != nullptr);
        REQUIRE(code_line[0].instructionCode() == code->getValue());
    }
}
