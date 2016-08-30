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
        auto data_type = compiler.compileExpression();

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
            "run error at line 1:7: overflow\n"
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
    SECTION("check for an error for a negative argument")
    {
        std::istringstream iss {"PRINT SQR(-6.25)"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:7: square root of negative number\n"
            "    PRINT SQR(-6.25)\n"
            "          ^^^\n");
    }
    SECTION("execute with an integer argument")
    {
        std::istringstream iss {"PRINT SQR(625)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "25\n");
    }
}


TEST_CASE("compile interger function expressions", "[int][compile]")
{
    ProgramUnit program;

    SECTION("make sure function and argument are parsed")
    {
        Compiler compiler {"INT(6.25)", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate integer function expressions", "[int][recreate]")
{
    ProgramUnit program;

    SECTION("recreate function with a double argument")
    {
        std::istringstream iss {"PRINT INT(-6.25)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT INT(-6.25)\n");
    }
}

TEST_CASE("execute integer function expressions", "[int][execute]")
{
    ProgramUnit program;

    SECTION("execute with positive arguments")
    {
        std::istringstream iss {"PRINT INT(6.01)\nPRINT INT(6.99)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "6\n6\n");
    }
    SECTION("execute with negative arguments")
    {
        std::istringstream iss {"PRINT INT(-6.01)\nPRINT INT(-6.99)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-7\n-7\n");
    }
}


TEST_CASE("compile fix function expressions", "[fix]")
{
    ProgramUnit program;

    SECTION("make sure function and argument are parsed")
    {
        Compiler compiler {"FIX(6.25)", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("recreate function with a double argument")
    {
        std::istringstream iss {"PRINT FIX(-6.25)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT FIX(-6.25)\n");
    }
    SECTION("execute with positive arguments")
    {
        std::istringstream iss {"PRINT FIX(6.01)\nPRINT FIX(6.99)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "6\n6\n");
    }
    SECTION("execute with negative arguments")
    {
        std::istringstream iss {"PRINT FIX(-6.01)\nPRINT FIX(-6.99)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-6\n-6\n");
    }
}


TEST_CASE("fractional function expressions", "[frac]")
{
    ProgramUnit program;

    SECTION("make sure function and argument are parsed")
    {
        Compiler compiler {"FRAC(6.25)", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("recreate function with a double argument")
    {
        std::istringstream iss {"PRINT FRAC(-6.25)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT FRAC(-6.25)\n");
    }
    SECTION("execute with positive arguments")
    {
        std::istringstream iss {"PRINT FRAC(6.01)\nPRINT FRAC(6.99)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0.01\n0.99\n");
    }
    SECTION("execute with negative arguments")
    {
        std::istringstream iss {"PRINT FRAC(-6.01)\nPRINT FRAC(-6.99)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-0.01\n-0.99\n");
    }
}


TEST_CASE("cosine function expressions", "[cos]")
{
    ProgramUnit program;

    SECTION("make sure function and argument are parsed")
    {
        Compiler compiler {"COS(0.9)", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("recreate function with a double argument")
    {
        std::istringstream iss {"PRINT COS(0.9)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT COS(0.9)\n");
    }
    SECTION("execute function with a double argument")
    {
        std::istringstream iss {"PRINT COS(0.9)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0.62161\n");
    }
}


TEST_CASE("sine function expressions", "[sin]")
{
    ProgramUnit program;

    SECTION("make sure function and argument are parsed")
    {
        Compiler compiler {"SIN(0.9)", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("recreate function with a double argument")
    {
        std::istringstream iss {"PRINT sin(0.9)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT SIN(0.9)\n");
    }
    SECTION("execute function with a double argument")
    {
        std::istringstream iss {"PRINT SIN(0.9)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0.783327\n");
    }
}


TEST_CASE("tangent function expressions", "[tan]")
{
    ProgramUnit program;

    SECTION("make sure function and argument are parsed")
    {
        Compiler compiler {"TAN(0.9)", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("recreate function with a double argument")
    {
        std::istringstream iss {"PRINT tan(0.9)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT TAN(0.9)\n");
    }
    SECTION("execute function with a double argument")
    {
        std::istringstream iss {"PRINT TAN(0.9)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "1.26016\n");
    }
}


TEST_CASE("arctangent function expressions", "[atn]")
{
    ProgramUnit program;

    SECTION("make sure function and argument are parsed")
    {
        Compiler compiler {"ATN(0.9)", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("recreate function with a double argument")
    {
        std::istringstream iss {"PRINT atn(0.9)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT ATN(0.9)\n");
    }
    SECTION("execute function with a double argument")
    {
        std::istringstream iss {"PRINT ATN(0.9)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0.732815\n");
    }
}


TEST_CASE("natural logarithm function expressions", "[log]")
{
    ProgramUnit program;

    SECTION("make sure function and argument are parsed")
    {
        Compiler compiler {"LOG(0.9)", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("recreate function with a double argument")
    {
        std::istringstream iss {"PRINT log(0.9)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT LOG(0.9)\n");
    }
    SECTION("execute function with a double argument")
    {
        std::istringstream iss {"PRINT LOG(0.9)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-0.105361\n");
    }
    SECTION("execute function with zero argument")
    {
        std::istringstream iss {"PRINT LOG(0.0)"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:7: logarithm of non-positive number\n"
            "    PRINT LOG(0.0)\n"
            "          ^^^\n");
    }
    SECTION("execute function with negative argument")
    {
        std::istringstream iss {"PRINT LOG(-0.9)"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:7: logarithm of non-positive number\n"
            "    PRINT LOG(-0.9)\n"
            "          ^^^\n");
    }
}


TEST_CASE("base-e exponential function expressions", "[exp]")
{
    ProgramUnit program;

    SECTION("make sure function and argument are parsed")
    {
        Compiler compiler {"EXP(0.9)", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("recreate function with a double argument")
    {
        std::istringstream iss {"PRINT exp(0.9)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT EXP(0.9)\n");
    }
    SECTION("execute function with a double argument")
    {
        std::istringstream iss {"PRINT EXP(0.9)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "2.4596\n");
    }
    SECTION("check execution of underflow")
    {
        std::istringstream iss {"PRINT EXP(-1e308)"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() == "0\n");
    }
    SECTION("check execution of overflow")
    {
        std::istringstream iss {"PRINT EXP(1e308)"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:7: overflow\n"
            "    PRINT EXP(1e308)\n"
            "          ^^^\n");
    }
}


TEST_CASE("compile convert to double function expressions", "[cdbl][compile]")
{
    ProgramUnit program;

    SECTION("make sure function and argument are parsed")
    {
        Compiler compiler {"CDBL(123)", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check that the function code is added after the operand")
    {
        Compiler compiler {"CDBL(1+1)", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        extern Code cdbl_code;
        REQUIRE(code_line.size() == 6);
        REQUIRE(code_line[5].instructionCode()->getValue() == cdbl_code.getValue());
    }
    SECTION("check that the function has the correct return data type")
    {
        Compiler compiler {"CDBL(123)", program};
        auto data_type = compiler.compileExpression();

        REQUIRE(data_type == DataType::Double);
    }
    SECTION("check for an internal convert to integer code for an integer argument")
    {
        Compiler compiler {"CDBL(1.0+2.1)", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        extern Code cvtint_code;
        extern Code cdbl_code;
        REQUIRE(code_line.size() == 7);
        REQUIRE(code_line[5].instructionCode()->getValue() == cvtint_code.getValue());
        REQUIRE(code_line[6].instructionCode()->getValue() == cdbl_code.getValue());
    }
}

TEST_CASE("recreate convert to double function expressions", "[cdbl][recreate]")
{
    ProgramUnit program;

    SECTION("recreate function with a integer argument")
    {
        std::istringstream iss {"print cdbl(1+1)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT CDBL(1 + 1)\n");
    }
}

TEST_CASE("execute convert to double funciton expressions", "[cdbl][execute]")
{
    ProgramUnit program;

    SECTION("execute with an integer expression")
    {
        std::istringstream iss {"PRINT CDBL(100+20+3)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "123\n");
    }
}


TEST_CASE("compile convert to integer function expressions", "[cint][compile]")
{
    ProgramUnit program;

    SECTION("make sure function and argument are parsed")
    {
        Compiler compiler {"CINT(123.4)", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check that the function code is added after the operand")
    {
        Compiler compiler {"CINT(1.0+2.1)", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        extern Code cint_code;
        REQUIRE(code_line.size() == 6);
        REQUIRE(code_line[5].instructionCode()->getValue() == cint_code.getValue());
    }
    SECTION("check that the function has the correct return data type")
    {
        Compiler compiler {"CINT(123.4)", program};
        auto data_type = compiler.compileExpression();

        REQUIRE(data_type == DataType::Integer);
    }
}

TEST_CASE("recreate convert to integer function expressions", "[cint][recreate]")
{
    ProgramUnit program;

    SECTION("recreate function with a integer argument")
    {
        std::istringstream iss {"print cint(123.4)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT CINT(123.4)\n");
    }
}

TEST_CASE("execute convert to integer funciton expressions", "[cint][execute]")
{
    ProgramUnit program;

    SECTION("execute with a double argument")
    {
        std::istringstream iss {"PRINT CINT(123.4)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "123\n");
    }
    SECTION("check that values are rounded up when required")
    {
        std::istringstream iss {"PRINT CINT(123.5)"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "124\n");
    }
}
