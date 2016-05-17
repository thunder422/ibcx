/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "constnumparser.h"
#include "parseerror.h"
#include "programcode.h"
#include "programunit.h"


TEST_CASE("parsing integer constants from a string", "[integers]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("parse a single digit number")
    {
        extern Code constIntCode;

        std::istringstream iss("1");
        auto data_type = ConstNumParser(iss).getCode(program, code_line);
        REQUIRE(data_type == DataType::Integer);
        REQUIRE(code_line.size() == 2);
        REQUIRE(code_line[0].instructionCode() == constIntCode.getValue());
        auto operand = code_line[1].operand();
        REQUIRE(program.constIntDictionary().get(operand) == "1");
    }
    SECTION("parse a multiple digit number")
    {
        std::istringstream iss("123");
        auto data_type = ConstNumParser(iss).getCode(program, code_line);
        REQUIRE(code_line.size() == 2);
        auto operand = code_line[1].operand();
        REQUIRE(program.constIntDictionary().get(operand) == "123");
    }
    SECTION("parse a negative number")
    {
        std::istringstream iss("-234");
        auto data_type = ConstNumParser(iss).getCode(program, code_line);
        REQUIRE(code_line.size() == 2);
        auto operand = code_line[1].operand();
        REQUIRE(program.constIntDictionary().get(operand) == "-234");
    }
    SECTION("terminate parsing at correct character")
    {
        std::istringstream iss("345+");
        auto data_type = ConstNumParser(iss).getCode(program, code_line);
        REQUIRE(code_line.size() == 2);
        auto operand = code_line[1].operand();
        REQUIRE(program.constIntDictionary().get(operand) == "345");
        REQUIRE(iss.peek() == '+');
    }
}


TEST_CASE("parsing floating point constants from a string", "[doubles]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("parse a number with a decimal point")
    {
        std::istringstream iss("0.5");
        auto data_type = ConstNumParser(iss).getCode(program, code_line);
        REQUIRE(data_type == DataType::Double);
    }
    SECTION("parse a number with a decimal point at the beginning")
    {
        std::istringstream iss(".75");
        auto data_type = ConstNumParser(iss).getCode(program, code_line);
        REQUIRE(data_type == DataType::Double);
    }
    SECTION("parse a number with a second decimal point (should ignore second one)")
    {
        std::istringstream iss("0.1.");
        auto data_type = ConstNumParser(iss).getCode(program, code_line);
        REQUIRE(data_type == DataType::Double);
        REQUIRE(iss.peek() == '.');
    }
    SECTION("parse a number to a double constant in the code_line")
    {
        extern Code constDblCode;

        std::istringstream iss("1.2");
        auto data_type = ConstNumParser(iss).getCode(program, code_line);
        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 2);
        REQUIRE(code_line[0].instructionCode() == constDblCode.getValue());
        auto operand = code_line[1].operand();
        REQUIRE(program.constDblDictionary().get(operand) == "1.2");
    }
    SECTION("parse a number with an exponent")
    {
        std::istringstream iss("1e0");
        auto data_type = ConstNumParser(iss).getCode(program, code_line);
        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 2);
        auto operand = code_line[1].operand();
        REQUIRE(program.constDblDictionary().get(operand) == "1e0");
    }
    SECTION("make sure parsing stops before a second 'E'")
    {
        std::istringstream iss("1e0E");
        auto data_type = ConstNumParser(iss).getCode(program, code_line);
        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 2);
        auto operand = code_line[1].operand();
        REQUIRE(program.constDblDictionary().get(operand) == "1e0");
        REQUIRE(iss.peek() == 'E');
    }
    SECTION("parse a number with a minus exponent")
    {
        std::istringstream iss("1e-2");
        auto data_type = ConstNumParser(iss).getCode(program, code_line);
        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 2);
        auto operand = code_line[1].operand();
        REQUIRE(program.constDblDictionary().get(operand) == "1e-2");
    }
    SECTION("parse a number with a minus exponent terminated be a minus operator")
    {
        std::istringstream iss("1e-2-");
        auto data_type = ConstNumParser(iss).getCode(program, code_line);
        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 2);
        auto operand = code_line[1].operand();
        REQUIRE(program.constDblDictionary().get(operand) == "1e-2");
        REQUIRE(iss.peek() == '-');
    }
    SECTION("parse a number with a plus exponent")
    {
        std::istringstream iss("1e+2");
        auto data_type = ConstNumParser(iss).getCode(program, code_line);
        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 2);
        auto operand = code_line[1].operand();
        REQUIRE(program.constDblDictionary().get(operand) == "1e+2");
    }
}


TEST_CASE("check for various number constant parsing errors", "[errors]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("error from an unexpected symbol at the beginning")
    {
        std::istringstream iss("%");
        REQUIRE_THROWS_AS(ConstNumParser(iss).getCode(program, code_line), ParseError);
    }
    SECTION("error message for an unexpected symbol at the beginning")
    {
        std::istringstream iss("%");
        try {
            ConstNumParser(iss).getCode(program, code_line);
        }
        catch (const ParseError &error) {
            std::string expected = "expected numerical constant or unary operator";
            REQUIRE(error.what() == expected);
        }
    }
}
