/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "compileerror.h"
#include "compiler.h"
#include "programerror.h"
#include "programunit.h"


TEST_CASE("compile negate operator expressions", "[compile]")
{
    ProgramUnit program;

    SECTION("make sure operand and constant operand are parsed")
    {
        Compiler compiler {"--2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check that a double negate code is added after the operand")
    {
        Compiler compiler {"--2.0", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        extern Code neg_dbl_code;
        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 3);
        REQUIRE(code_line[2].instructionCode()->getValue() == neg_dbl_code.getValue());
    }
    SECTION("check that an integer negate code is added after the operand")
    {
        Compiler compiler {"--2", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        extern Code neg_int_code;
        REQUIRE(data_type == DataType::Integer);
        REQUIRE(code_line.size() == 3);
        REQUIRE(code_line[2].instructionCode()->getValue() == neg_int_code.getValue());
    }
    SECTION("verify an error is thrown when nothing is after the negate operator")
    {
        Compiler compiler {"-", program};

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(compiler.compileExpression(DataType::Null), CompileError);
        }
        SECTION("check the message, column and length of the error thrown")
        {
            try {
                compiler.compileExpression(DataType::Null);
            }
            catch (const CompileError &error) {
                REQUIRE(error.what() == std::string("expected numeric expression"));
                REQUIRE(error.column == 1);
                REQUIRE(error.length == 1);
            }
        }
    }
    SECTION("check that white space is allowed after a negate operator")
    {
        Compiler compiler {"- 2", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        extern Code neg_int_code;
        REQUIRE(compiler.peekNextChar() == EOF);
        REQUIRE(code_line.size() == 3);
        REQUIRE(code_line[2].instructionCode()->getValue() == neg_int_code.getValue());
    }
}

TEST_CASE("recreate negate operator expressions", "[recreate]")
{
    ProgramUnit program;

    SECTION("recreate a negation of a negative integer constant")
    {
        std::istringstream iss {"PRINT --2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT --2\n");
    }
    SECTION("recreate a negation of a positve integer constant")
    {
        std::istringstream iss {"PRINT - 2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT - 2\n");
    }
    SECTION("recreate a negation of a negative double constant")
    {
        std::istringstream iss {"PRINT --2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT --2.0\n");
    }
    SECTION("recreate a negation of a positive double constant starting with a decimal point")
    {
        std::istringstream iss {"PRINT - .1"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT - .1\n");
    }
}

TEST_CASE("execute negate operator expressions", "[execute]")
{
    ProgramUnit program;

    SECTION("execute a negation of an integer constant")
    {
        std::istringstream iss {"PRINT --345"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "345\n");
    }
    SECTION("execute a negation of an double constant")
    {
        std::istringstream iss {"PRINT - 1.345e210"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-1.345e+210\n");
    }
    SECTION("check that exponential is higher precedence than negation")
    {
        std::istringstream iss {"PRINT 2.0^- 2^3"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0.00390625\n");
    }
}

