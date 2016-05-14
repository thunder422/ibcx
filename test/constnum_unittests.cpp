/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "constnumparser.h"


TEST_CASE("parsing integers from a string", "[integers]")
{
    SECTION("not a number in string (null token pointer)")
    {
        std::istringstream iss("A");
        auto token = ConstNumParser(iss).getToken();
        REQUIRE_FALSE(token);
    }
}
