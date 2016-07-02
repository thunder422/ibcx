/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "compileerror.h"
#include "compiler.h"
#include "operators.h"
#include "programerror.h"
#include "programunit.h"


TEST_CASE("compile exponential operator expressions", "[compile]")
{
    ProgramUnit program;

    SECTION("make sure both operands and operator are parsed")
    {
        Compiler compiler {"3^2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("make sure the operator is the correct character")
    {
        Compiler compiler {"3a2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == 'a');
    }
    SECTION("make sure white space is allowed before the operator")
    {
        Compiler compiler {"3 ^2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("make sure white space is allowed after the operator")
    {
        Compiler compiler {"3 ^ 2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("make sure parsing stops if there is no operand")
    {
        Compiler compiler {"^2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == '^');
    }
    SECTION("check for an error if there is a problem with the second operand")
    {
        Compiler compiler {"3^^", program};

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(compiler.compileExpression(DataType::Null), ExpNumExprError);
        }
        SECTION("check the message, column and length of the error thrown")
        {
            try {
                compiler.compileExpression(DataType::Null);
            }
            catch (const ExpNumExprError &error) {
                REQUIRE(error.column == 2);
                REQUIRE(error.length == 1);
            }
        }
    }
    SECTION("check for an integer exponential code at end of code line")
    {
        extern OperatorCode<OpType::IntInt> exp_int_int_code;

        Compiler compiler {"3^2", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Integer);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == exp_int_int_code.getValue());
    }
    SECTION("check for an double exponential code at end of code line")
    {
        extern OperatorCode<OpType::DblDbl> exp_dbl_dbl_code;

        Compiler compiler {"3.0^2.0", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == exp_dbl_dbl_code.getValue());
    }
    SECTION("check for an right side integer exponential code at end of code line")
    {
        extern OperatorCode<OpType::DblInt> exp_dbl_int_code;

        Compiler compiler {"3.0^2", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == exp_dbl_int_code.getValue());
    }
    SECTION("check for an left side integer exponential code at end of code line")
    {
        extern OperatorCode<OpType::IntDbl> exp_int_dbl_code;

        Compiler compiler {"3^2.0", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == exp_int_dbl_code.getValue());
    }
    SECTION("check that multiple exponential operators are compiled")
    {
        extern OperatorCode<OpType::IntDbl> exp_int_dbl_code;
        extern OperatorCode<OpType::DblInt> exp_dbl_int_code;

        Compiler compiler {"3^2.0^4", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate exponential operator expression", "[recreate]")
{
    ProgramUnit program;

    SECTION("recreate a exponential with two integer constants")
    {
        std::istringstream iss {"PRINT 3^2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 ^ 2\n");
    }
    SECTION("recreate a exponential with two double constants")
    {
        std::istringstream iss {"PRINT 3.0^2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 ^ 2.0\n");
    }
    SECTION("recreate a exponential with one integer and one double constant")
    {
        std::istringstream iss {"PRINT 3^2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 ^ 2.0\n");
    }
    SECTION("recreate a exponential with one double and one integer constant")
    {
        std::istringstream iss {"PRINT 3.0^2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 ^ 2\n");
    }
}

TEST_CASE("execute exponential operator expressions", "[execute]")
{
    ProgramUnit program;

    SECTION("execute an exponential of two integer constants")
    {
        std::istringstream iss {"PRINT 3^2"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "9\n");
    }
    SECTION("execute an exponential of two different integer constants")
    {
        std::istringstream iss {"PRINT 2^5"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "32\n");
    }
    SECTION("execute an exponential of two integer constants with a negative exponent")
    {
        std::istringstream iss {"PRINT 2^-5"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0\n");
    }
    SECTION("execute an exponential of two integer constants with one to a negative exponent")
    {
        std::istringstream iss {"PRINT 1^-5"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "1\n");
    }
    SECTION("execute an exponential of two integer constants with one to a negative exponent")
    {
        std::istringstream iss {"PRINT -1^-5"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-1\n");
    }
    SECTION("execute an exponential of two integer constants with one to an even negative exponent")
    {
        std::istringstream iss {"PRINT -1^-6"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "1\n");
    }
    SECTION("execute an exponential of two integer constants with one to an even negative exponent")
    {
        std::istringstream iss {"PRINT 0^4^-1"};
        std::ostringstream oss;

        program.compile(iss);

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(program.run(oss), ProgramError);
        }
        SECTION("check the offset of the error thrown")
        {
            try {
                program.run(oss);
            }
            catch (const ProgramError &error) {
                std::string expected = "divide by zero";
                REQUIRE(error.what() == expected);
                REQUIRE(error.line_number == 1);
                REQUIRE(error.column == 12);
                REQUIRE(error.length == 1);
                REQUIRE(error.line == "PRINT 0 ^ 4 ^ -1");
            }
        }
    }
    SECTION("execute an exponential that causes an overflow")
    {
        std::istringstream iss {"PRINT 16^8"};
        std::ostringstream oss;

        program.compile(iss);

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(program.run(oss), ProgramError);
        }
        SECTION("check that the correct error information is thrown")
        {
            REQUIRE_FALSE(program.runCode(oss));
            REQUIRE(oss.str() ==
                "run error at line 1:9: overflow\n"
                "    PRINT 16 ^ 8\n"
                "             ^\n");
        }
    }
    SECTION("execute an exponential of a negative number that causes a negative overflow")
    {
        std::istringstream iss {"PRINT -8^11"};
        std::ostringstream oss;

        program.compile(iss);

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(program.run(oss), ProgramError);
        }
        SECTION("check that the correct error information is thrown")
        {
            REQUIRE_FALSE(program.runCode(oss));
            REQUIRE(oss.str() ==
                "run error at line 1:9: overflow\n"
                "    PRINT -8 ^ 11\n"
                "             ^\n");
        }
    }
    SECTION("execute an exponential of a negative number that causes a positive overflow")
    {
        std::istringstream iss {"PRINT -4^16"};
        std::ostringstream oss;

        program.compile(iss);

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(program.run(oss), ProgramError);
        }
        SECTION("check that the correct error information is thrown")
        {
            REQUIRE_FALSE(program.runCode(oss));
            REQUIRE(oss.str() ==
                "run error at line 1:9: overflow\n"
                "    PRINT -4 ^ 16\n"
                "             ^\n");
        }
    }
    SECTION("execute an exponential of a negative number")
    {
        std::istringstream iss {"PRINT -9^3"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() == "-729\n");
    }
    SECTION("execute alternate double power for high exponent")
    {
        std::istringstream iss {"PRINT 2^19"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() == "524288\n");

    }
}
