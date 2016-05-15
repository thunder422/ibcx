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


TEST_CASE("parsing integer constants from a string", "[integers]")
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
    SECTION("parse a single digit number")
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
    SECTION("parse a multiple digit number")
    {
        std::istringstream iss("123");
        auto data_type = ConstNumParser(iss).getCode(code, program);
        REQUIRE(code.size() == 2);
        auto operand = code[1].operand();
        REQUIRE(program.constIntDictionary().get(operand) == "123");
    }
    SECTION("parse a negative number")
    {
        std::istringstream iss("-234");
        auto data_type = ConstNumParser(iss).getCode(code, program);
        REQUIRE(code.size() == 2);
        auto operand = code[1].operand();
        REQUIRE(program.constIntDictionary().get(operand) == "-234");
    }
    SECTION("terminate parsing at correct character")
    {
        std::istringstream iss("345+");
        auto data_type = ConstNumParser(iss).getCode(code, program);
        REQUIRE(code.size() == 2);
        auto operand = code[1].operand();
        REQUIRE(program.constIntDictionary().get(operand) == "345");
        REQUIRE(iss.peek() == '+');
    }
}


TEST_CASE("parsing floating point constants from a string", "[doubles]")
{
    ProgramModel program;
    ProgramCode code;

    SECTION("parse a number with a decimal point")
    {
        std::istringstream iss("0.5");
        auto data_type = ConstNumParser(iss).getCode(code, program);
        REQUIRE(data_type == DataType::Double);
    }
    SECTION("parse a number with a decimal point at the beginning")
    {
        std::istringstream iss(".75");
        auto data_type = ConstNumParser(iss).getCode(code, program);
        REQUIRE(data_type == DataType::Double);
    }
    SECTION("parse a number with a second decimal point (should ignore second one)")
    {
        std::istringstream iss("0.1.");
        auto data_type = ConstNumParser(iss).getCode(code, program);
        REQUIRE(data_type == DataType::Double);
        REQUIRE(iss.peek() == '.');
    }
    SECTION("parse a number to a double constant in the code")
    {
        extern Code constDblCode;

        std::istringstream iss("1.2");
        auto data_type = ConstNumParser(iss).getCode(code, program);
        REQUIRE(data_type == DataType::Double);
        REQUIRE(code.size() == 2);
        REQUIRE(code[0].instructionCode() == constDblCode.getValue());
    }
}
