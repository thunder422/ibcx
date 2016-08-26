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


TEST_CASE("compile absolute function expressions", "[abs][compile]")
{
    ProgramUnit program;

    SECTION("make sure function and argument are parsed")
    {
        Compiler compiler {"ABS(-2)", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check that the double function code is added after the operand")
    {
        Compiler compiler {"ABS(-2.1)", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        extern Code abs_dbl_code;
        REQUIRE(code_line.size() == 3);
        REQUIRE(code_line[2].instructionCode()->getValue() == abs_dbl_code.getValue());
    }
    SECTION("check for a double return data type")
    {
        Compiler compiler {"ABS(-2.1)", program};
        auto data_type = compiler.compileExpression(DataType::Null);

        REQUIRE(data_type == DataType::Double);
    }
    SECTION("check that the integer function code is added after the operand")
    {
        Compiler compiler {"ABS(-2)", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        extern Code abs_int_code;
        REQUIRE(code_line.size() == 3);
        REQUIRE(code_line[2].instructionCode()->getValue() == abs_int_code.getValue());
    }
    SECTION("check for an error if there is no opening parenthesis")
    {
        Compiler compiler {"ABS -2.1", program};

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
                std::string expected_what = "expected opening parentheses";
                REQUIRE(error.what() == expected_what);
                REQUIRE(error.column == 4);
                REQUIRE(error.length == 1);
            }
        }
    }
}

TEST_CASE("recreate abs function expressions", "[abs][recreate]")
{
    ProgramUnit program;

    SECTION("recreate function with a double argument")
    {
        std::istringstream iss {"PRINT ABS(-2.1)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT ABS(-2.1)\n");
    }
    SECTION("recreate function with an integer argument")
    {
        std::istringstream iss {"PRINT ABS  (-2)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT ABS(-2)\n");
    }
}

TEST_CASE("execute abs function expressions", "[abs][execute]")
{
    ProgramUnit program;

    SECTION("execute with a positive double argument")
    {
        std::istringstream iss {"PRINT ABS(2.1)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "2.1\n");
    }
    SECTION("execute with a positive double argument")
    {
        std::istringstream iss {"PRINT ABS(-2.1)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "2.1\n");
    }
    SECTION("execute with a positive integer argument")
    {
        std::istringstream iss {"PRINT ABS(2)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "2\n");
    }
    SECTION("execute with a positive double argument")
    {
        std::istringstream iss {"PRINT ABS(-2)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "2\n");
    }
    SECTION("check for an overflow with maxmimum negative integer argument")
    {
        std::istringstream iss {"PRINT ABS(-2147483648)"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:6: overflow\n"
            "    PRINT ABS(-2147483648)\n"
            "          ^^^\n");
    }
}


TEST_CASE("compile sign function expressions", "[sgn][compile]")
{
    ProgramUnit program;

    SECTION("make sure function and argument are parsed")
    {
        Compiler compiler {"SGN(-2)", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate sgn function expressions", "[sgn][recreate]")
{
    ProgramUnit program;

    SECTION("recreate function with a double argument")
    {
        std::istringstream iss {"PRINT SGN(-2.1)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT SGN(-2.1)\n");
    }
    SECTION("recreate function with an integer argument")
    {
        std::istringstream iss {"PRINT SGN(-2)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT SGN(-2)\n");
    }
}

TEST_CASE("execute sgn function expressions", "[sgn][execute]")
{
    ProgramUnit program;

    SECTION("execute with a zero double argument")
    {
        std::istringstream iss {"PRINT SGN(0.0)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0\n");
    }
    SECTION("execute with a positve double argument")
    {
        std::istringstream iss {"PRINT SGN(7.3)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "1\n");
    }
    SECTION("execute with a negative double argument")
    {
        std::istringstream iss {"PRINT SGN(-3.7)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-1\n");
    }
    SECTION("execute with a zero integer argument")
    {
        std::istringstream iss {"PRINT SGN(0)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0\n");
    }
    SECTION("execute with a positve integer argument")
    {
        std::istringstream iss {"PRINT SGN(7)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "1\n");
    }
    SECTION("execute with a negative integer argument")
    {
        std::istringstream iss {"PRINT SGN(-3)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-1\n");
    }
}


TEST_CASE("compile square root function expressions", "[sqr][compile]")
{
    ProgramUnit program;

    SECTION("make sure function and argument are parsed")
    {
        Compiler compiler {"SQR(6.25)", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate square root function expressions", "[sqr][recreate]")
{
    ProgramUnit program;

    SECTION("recreate function with a double argument")
    {
        std::istringstream iss {"PRINT SQR(-6.25)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT SQR(-6.25)\n");
    }
}

TEST_CASE("execute square root function expressions", "[sqr][execute]")
{
    ProgramUnit program;

    SECTION("execute with a double argument")
    {
        std::istringstream iss {"PRINT SQR(6.25)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "2.5\n");
    }
}
