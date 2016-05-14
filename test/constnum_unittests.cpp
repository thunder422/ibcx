/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "constnumparser.h"
#include "programcode.h"
#include "programmodel.h"


TEST_CASE("parsing integers from a string", "[integers]")
{
    ProgramModel program;
    ProgramCode code;

    SECTION("not a number in string (null token pointer)")
    {
        std::istringstream iss("A");
        auto data_type = ConstNumParser(iss).getCode(code, program);
        REQUIRE(data_type == DataType::Null);
        REQUIRE(code.empty());
    }
    SECTION("parse a single digit number (integer)")
    {
        extern Code constIntCode;

        std::istringstream iss("1");
        auto data_type = ConstNumParser(iss).getCode(code, program);
        REQUIRE(data_type == DataType::Integer);
        REQUIRE(code.size() == 2);
        REQUIRE(code[0].instructionCode() == constIntCode.getValue());
        auto operand = code[1].operand();
        REQUIRE(program.constIntDictionary().get(operand) == "1");
    }
}
