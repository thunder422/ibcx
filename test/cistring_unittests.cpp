/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "cistring.h"

TEST_CASE("test case insensitive string class", "[test]")
{
    SECTION("comparisons with standard strings")
    {
        ci_string string1 = "Print";
        std::string string2 = "pRInt";

        REQUIRE(string1 == string2);
        REQUIRE(string2 == string1);
        REQUIRE(string1 <= string2);
        REQUIRE(string2 <= string1);
        REQUIRE(string1 >= string2);
        REQUIRE(string2 >= string1);
        REQUIRE_FALSE(string1 != string2);
        REQUIRE_FALSE(string2 != string1);
        REQUIRE_FALSE(string1 < string2);
        REQUIRE_FALSE(string2 < string1);
        REQUIRE_FALSE(string1 > string2);
        REQUIRE_FALSE(string2 > string1);
    }
    SECTION("comparisons with c-style strings")
    {
        ci_string string1 = "End";
        const char *string2 = "enD";

        REQUIRE(string1 == string2);
        REQUIRE(string2 == string1);
        REQUIRE(string1 <= string2);
        REQUIRE(string1 >= string2);
        REQUIRE_FALSE(string1 != string2);
        REQUIRE_FALSE(string2 != string1);
        REQUIRE_FALSE(string1 < string2);
        REQUIRE_FALSE(string1 > string2);
    }
    SECTION("comparisons with standard strings containing underscores")
    {
        ci_string string1_high = "var";
        std::string string2_low = "V_V";
        ci_string string3_low = "v_v";
        std::string string4_high = "Var";

        REQUIRE(string1_high > string2_low);
        REQUIRE(string3_low < string4_high);
        REQUIRE(string1_high != string2_low);
        REQUIRE(string1_high >= string2_low);
        REQUIRE_FALSE(string1_high < string2_low);
        REQUIRE_FALSE(string1_high <= string2_low);
        REQUIRE_FALSE(string1_high == string2_low);
    }
    SECTION("find character in string")
    {
        ci_string string = "aBcDeFgH";

        REQUIRE(string.find_first_of('d') == 3);
        REQUIRE(string.find_first_of('F') == 5);
    }
    SECTION("output case insensitive string to a stream")
    {
        ci_string output_string = "Output";

        std::ostringstream oss;
        oss << output_string;

        REQUIRE(oss.str() == "Output");
    }
    SECTION("input case insensitive string from a stream")
    {
        std::istringstream iss("Input");

        ci_string input_string;
        iss >> input_string;

        REQUIRE(input_string == "Input");
    }
    SECTION("setting case insensitive string from a standard string")
    {
        std::string std_string = "Test";
        ci_string cs = std_string.c_str();

        REQUIRE(cs == "Test");
    }
}
