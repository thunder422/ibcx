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


#define REQUIRE_INTEGER_OPERAND(number) \
    REQUIRE(data_type == DataType::Integer); \
    REQUIRE(code_line.size() == 2); \
    auto operand = code_line[1].operand(); \
    REQUIRE(program.constIntDictionary().get(operand) == number);

#define REQUIRE_DOUBLE_OPERAND(number) \
    REQUIRE(data_type == DataType::Double); \
    REQUIRE(code_line.size() == 2); \
    auto operand = code_line[1].operand(); \
    REQUIRE(program.constDblDictionary().get(operand) == number);


TEST_CASE("parsing integer constants from a string", "[integers]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("input stream does not contain a constant (caller will determine action)")
    {
        std::istringstream iss {"%"};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE(data_type == DataType::Null);
        REQUIRE(code_line.size() == 0);
        REQUIRE(iss.peek() == '%');
    }
    SECTION("parse a single digit number")
    {
        extern Code constIntCode;

        std::istringstream iss {"1"};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE_INTEGER_OPERAND("1");
        REQUIRE(code_line[0].instructionCode() == constIntCode.getValue());
    }
    SECTION("parse a multiple digit number")
    {
        std::istringstream iss {"123"};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE_INTEGER_OPERAND("123");
    }
    SECTION("parse a negative number")
    {
        std::istringstream iss {"-234"};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE_INTEGER_OPERAND("-234");
    }
    SECTION("terminate parsing at correct character")
    {
        std::istringstream iss {"345+"};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE_INTEGER_OPERAND("345");
        REQUIRE(iss.peek() == '+');
    }
}


TEST_CASE("parsing floating point constants from a string", "[doubles]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("parse a number with a decimal point")
    {
        std::istringstream iss {"0.5"};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE_DOUBLE_OPERAND("0.5");
    }
    SECTION("parse a number with a decimal point at the beginning")
    {
        std::istringstream iss {".75"};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE_DOUBLE_OPERAND(".75");
    }
    SECTION("parse a number with a second decimal point (should ignore second one)")
    {
        std::istringstream iss {"0.1."};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE_DOUBLE_OPERAND("0.1");
        REQUIRE(iss.peek() == '.');
    }
    SECTION("parse a number to a double constant in the code_line")
    {
        extern Code constDblCode;

        std::istringstream iss {"1.2"};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE_DOUBLE_OPERAND("1.2");
        REQUIRE(code_line[0].instructionCode() == constDblCode.getValue());
    }
    SECTION("parse a number with an exponent")
    {
        std::istringstream iss {"1e0"};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE_DOUBLE_OPERAND("1e0");
    }
    SECTION("make sure parsing stops before a second 'E'")
    {
        std::istringstream iss {"1e0E"};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE_DOUBLE_OPERAND("1e0");
        REQUIRE(iss.peek() == 'E');
    }
    SECTION("parse a number with a minus exponent")
    {
        std::istringstream iss {"1e-2"};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE_DOUBLE_OPERAND("1e-2");
    }
    SECTION("parse a number with a minus exponent terminated be a minus operator")
    {
        std::istringstream iss {"1e-2-"};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE_DOUBLE_OPERAND("1e-2");
        REQUIRE(iss.peek() == '-');
    }
    SECTION("parse a number with a plus exponent")
    {
        std::istringstream iss {"1e+2"};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE_DOUBLE_OPERAND("1e+2");
    }
}


TEST_CASE("handle leading zero of a constant correctly including errors", "[zero]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("check for an error when a leading zero is not followed by a digit")
    {
        std::istringstream iss {"01"};
        REQUIRE_THROWS_AS(ConstNumParser{iss}.parse(code_line, program), ParseError);
    }
    SECTION("check for the correct error message and a column")
    {
        std::istringstream iss {"01"};
        try {
            ConstNumParser{iss}.parse(code_line, program);
        }
        catch (const ParseError &error) {
            std::string expected = "expected decimal point after leading zero";
            REQUIRE(error.what() == expected);
            REQUIRE(error.column == 1);
        }
    }
    SECTION("check for the correct error column")
    {
        std::istringstream iss {"word 01"};
        std::string skip_word;
        iss >> skip_word >> std::ws;
        try {
            ConstNumParser{iss}.parse(code_line, program);
        }
        catch (const ParseError &error) {
            std::string expected = "expected decimal point after leading zero";
            REQUIRE(error.what() == expected);
            REQUIRE(error.column == 6);
        }
    }
    SECTION("check parsing ends when followed by a non-period non-digit")
    {
        std::istringstream iss {"0-"};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE(data_type == DataType::Integer);
        REQUIRE_INTEGER_OPERAND("0");
        REQUIRE(iss.peek() == '-');
    }
}


TEST_CASE("handle leading period of a constant correctly including errors", "[period]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("check for an error when a leading period is followed by another period")
    {
        std::istringstream iss {".."};
        REQUIRE_THROWS_AS(ConstNumParser{iss}.parse(code_line, program), ParseError);
    }
    SECTION("check error message and column if no digits after period")
    {
        std::istringstream iss {".."};
        try {
            ConstNumParser{iss}.parse(code_line, program);
        }
        catch (const ParseError &error) {
            std::string expected = "expected digit after decimal point";
            REQUIRE(error.what() == expected);
            REQUIRE(error.column == 1);
        }
    }
}


TEST_CASE("check for correct exponent format", "[exponent]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("check for sign or digits at the start of an exponent")
    {
        std::istringstream iss {"1e."};
        REQUIRE_THROWS_AS(ConstNumParser{iss}.parse(code_line, program), ParseError);
    }
    SECTION("check error message and column if no sign or digits at start of exponent")
    {
        std::istringstream iss {"1e."};
        try {
            ConstNumParser{iss}.parse(code_line, program);
        }
        catch (const ParseError &error) {
            std::string expected = "expected sign or digit for exponent";
            REQUIRE(error.what() == expected);
            REQUIRE(error.column == 2);
        }
    }
    SECTION("allow for possible EQV operator ('E' lost, which will be handled by caller)")
    {
        std::istringstream iss {"1eq"};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE_INTEGER_OPERAND("1");
        REQUIRE(iss.peek() == 'q');
    }
    SECTION("make sure there is a digit after an exponent sign")
    {
        std::istringstream iss {"1e+"};
        REQUIRE_THROWS_AS(ConstNumParser{iss}.parse(code_line, program), ParseError);
    }
    SECTION("check error message and column if no digit after exponent sign")
    {
        std::istringstream iss {"1e-A"};
        try {
            ConstNumParser{iss}.parse(code_line, program);
        }
        catch (const ParseError &error) {
            std::string expected = "expected digit after exponent sign";
            REQUIRE(error.what() == expected);
            REQUIRE(error.column == 3);
        }
    }
    SECTION("check correct error column when terminated by end of line")
    {
        std::istringstream iss {"1e-"};
        try {
            ConstNumParser{iss}.parse(code_line, program);
        }
        catch (const ParseError &error) {
            REQUIRE(error.column == 3);
        }

    }
}


TEST_CASE("look for possible exit conditions", "[exit]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("look for possible negate operator ('-' not followed by '.' or digit)")
    {
        std::istringstream iss {"-e"};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE(data_type == DataType::Null);
        REQUIRE(code_line.size() == 0);
    }
    SECTION("allow a period after a negative sign")
    {
        std::istringstream iss {"-.1"};
        auto data_type = ConstNumParser{iss}.parse(code_line, program);
        REQUIRE_DOUBLE_OPERAND("-.1");
    }
    SECTION("look for negate operator status (false if not negate operator)")
    {
        std::istringstream iss {"-1-"};
        ConstNumParser number {iss};
        auto data_type = number.parse(code_line, program);
        REQUIRE_INTEGER_OPERAND("-1");
        REQUIRE(iss.peek() == '-');
        REQUIRE_FALSE(number.negateOperator());
    }
    SECTION("look for negate operator status (true for negate operator)")
    {
        std::istringstream iss {"-e"};
        ConstNumParser number {iss};
        auto data_type = number.parse(code_line, program);
        REQUIRE(data_type == DataType::Null);
        REQUIRE(iss.peek() == 'e');
        REQUIRE(number.negateOperator());
    }
    SECTION("look for possible operator status (false if no operator starting with 'E')")
    {
        std::istringstream iss {"-1e1-"};
        ConstNumParser number {iss};
        auto data_type = number.parse(code_line, program);
        REQUIRE_DOUBLE_OPERAND("-1e1");
        REQUIRE(iss.peek() == '-');
        REQUIRE_FALSE(number.possibleOperator());
    }
}
