/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "constnumparser.h"
#include "programcode.h"


TEST_CASE("parsing integers from a string", "[integers]")
{
    ProgramCode code;

    SECTION("not a number in string (null token pointer)")
    {
        std::istringstream iss("A");
        auto data_type = ConstNumParser(iss).getCode(code);
        REQUIRE(data_type == DataType::Null);
    }
    SECTION("parse a single digit number (integer)")
    {
        std::istringstream iss("1");
        auto data_type = ConstNumParser(iss).getCode(code);
        REQUIRE(data_type == DataType::Integer);
    }
}
