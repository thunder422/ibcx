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


TEST_CASE("compile negate operator expressions", "[neg][compile]")
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

TEST_CASE("recreate negate operator expressions", "[neg][recreate]")
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

TEST_CASE("execute negate operator expressions", "[neg][execute]")
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
    SECTION("execute a negation that causes an overflow")
    {
        std::istringstream iss {"PRINT --2147483648"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:6: overflow\n"
            "    PRINT --2147483648\n"
            "          ^\n");
    }
}


TEST_CASE("compile exponential operator expressions", "[exp][compile]")
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
    SECTION("check for an integer exponential code after the operands")
    {
        extern OperatorCode<OpType::IntInt> exp_int_int_code;

        Compiler compiler {"3^2", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Integer);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == exp_int_int_code.getValue());
    }
    SECTION("check for an double exponential code after the operands")
    {
        extern OperatorCode<OpType::DblDbl> exp_dbl_dbl_code;

        Compiler compiler {"3.0^2.0", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == exp_dbl_dbl_code.getValue());
    }
    SECTION("check for an right side integer exponential code after the operands")
    {
        extern OperatorCode<OpType::DblInt> exp_dbl_int_code;

        Compiler compiler {"3.0^2", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == exp_dbl_int_code.getValue());
    }
    SECTION("check for an left side integer exponential code after the operands")
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

TEST_CASE("recreate exponential operator expression", "[exp][recreate]")
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

TEST_CASE("execute integer-interger exponential operator", "[exp][int-int]")
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
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:9: overflow\n"
            "    PRINT 16 ^ 8\n"
            "             ^\n");
    }
    SECTION("execute an exponential of a negative number that causes a negative overflow")
    {
        std::istringstream iss {"PRINT -8^11"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:9: overflow\n"
            "    PRINT -8 ^ 11\n"
            "             ^\n");
    }
    SECTION("execute an exponential of a negative number that causes a positive overflow")
    {
        std::istringstream iss {"PRINT -4^16"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:9: overflow\n"
            "    PRINT -4 ^ 16\n"
            "             ^\n");
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
    SECTION("execute an exponential that causes an overflow with alternate double power")
    {
        std::istringstream iss {"PRINT 123^20"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:10: overflow\n"
            "    PRINT 123 ^ 20\n"
            "              ^\n");
    }
    SECTION("execute alternate double power for high exponent with a negative value")
    {
        std::istringstream iss {"PRINT -2^19"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() == "-524288\n");
    }
    SECTION("execute exponential with a negative value that causes an overflow with alternate")
    {
        std::istringstream iss {"PRINT -123^20"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:11: overflow\n"
            "    PRINT -123 ^ 20\n"
            "               ^\n");
    }
    SECTION("execute exponential with a negative value that causes an underflow with alternate")
    {
        std::istringstream iss {"PRINT -123^19"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:11: overflow\n"
            "    PRINT -123 ^ 19\n"
            "               ^\n");
    }
}

TEST_CASE("execute double-double exponential operator", "[exp][dbl-dbl]")
{
    ProgramUnit program;

    SECTION("execute an exponential of two double constants")
    {
        std::istringstream iss {"PRINT -3.0^2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "9\n");
    }
    SECTION("check for a domain error for a negative value raised to a non-integer exponent")
    {
        std::istringstream iss {"PRINT -3.0^2.5"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:11: domain error (non-integer exponent)\n"
            "    PRINT -3.0 ^ 2.5\n"
            "               ^\n");
    }
    SECTION("check for an overflow error if the result is too large")
    {
        std::istringstream iss {"PRINT 123.0^456.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:12: overflow\n"
            "    PRINT 123.0 ^ 456.0\n"
            "                ^\n");
    }
    SECTION("check for a divide by zero error if the result is infinity")
    {
        std::istringstream iss {"PRINT 0.0^-0.5"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:10: divide by zero\n"
            "    PRINT 0.0 ^ -0.5\n"
            "              ^\n");
    }
}

TEST_CASE("execute integer-double exponential operator", "[exp][int-dbl]")
{
    ProgramUnit program;

    SECTION("execute an exponential of an integer to a double")
    {
        std::istringstream iss {"PRINT -3^3.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-27\n");
    }
}

TEST_CASE("execute double-integer exponential operator", "[exp][dbl-int]")
{
    ProgramUnit program;

    SECTION("execute an exponential of a double to an integer")
    {
        std::istringstream iss {"PRINT -3.0^3"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-27\n");
    }
    SECTION("execute an exponential of a double to a negative integer")
    {
        std::istringstream iss {"PRINT 2.0^-4"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0.0625\n");
    }
    SECTION("check for a divide by zero error if the result is infinity")
    {
        std::istringstream iss {"PRINT 0.0^-1"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:10: divide by zero\n"
            "    PRINT 0.0 ^ -1\n"
            "              ^\n");
    }
    SECTION("check for an overflow error if the result is too large")
    {
        std::istringstream iss {"PRINT 1e307^4"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:12: overflow\n"
            "    PRINT 1e307 ^ 4\n"
            "                ^\n");
    }
    SECTION("execute alternate power for high exponents")
    {
        std::istringstream iss {"PRINT -2.0^19"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() == "-524288\n");
    }
    SECTION("overflow error if the result is too large with altenate power")
    {
        std::istringstream iss {"PRINT 123.0^456"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:12: overflow\n"
            "    PRINT 123.0 ^ 456\n"
            "                ^\n");
    }
    SECTION("execute alternate power for high negative exponents")
    {
        std::istringstream iss {"PRINT 2.0e6^-17"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() == "7.62939e-108\n");
    }
    SECTION("overflow error if result is too large with alternate power and negative exponents")
    {
        std::istringstream iss {"PRINT 2.0e-300^-17"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:15: overflow\n"
            "    PRINT 2.0e-300 ^ -17\n"
            "                   ^\n");
    }
}


TEST_CASE("compile multiply operator expressions", "[mul][compile]")
{
    ProgramUnit program;

    SECTION("make sure both operands and operator are parsed")
    {
        Compiler compiler {"3*2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check for an integer multiply code after the operands")
    {
        extern OperatorCode<OpType::IntInt> mul_int_int_code;

        Compiler compiler {"3*2", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Integer);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == mul_int_int_code.getValue());
    }
    SECTION("check for an double exponential code after the operands")
    {
        extern OperatorCode<OpType::DblDbl> mul_dbl_dbl_code;

        Compiler compiler {"3.0*2.0", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == mul_dbl_dbl_code.getValue());
    }
    SECTION("check for an right side integer multiply code after the operands")
    {
        extern OperatorCode<OpType::DblInt> mul_dbl_int_code;

        Compiler compiler {"3.0*2", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == mul_dbl_int_code.getValue());
    }
    SECTION("check for an left side integer multiply code after the operands")
    {
        extern OperatorCode<OpType::IntDbl> mul_int_dbl_code;

        Compiler compiler {"3*2.0", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == mul_int_dbl_code.getValue());
    }
    SECTION("check that multiple multiply operators are compiled")
    {
        extern OperatorCode<OpType::IntDbl> mul_int_dbl_code;
        extern OperatorCode<OpType::DblInt> mul_dbl_int_code;

        Compiler compiler {"3*2.0*4", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate multiply operator expressions", "[mul][recreate]")
{
    ProgramUnit program;

    SECTION("recreate a multiply with two integer constants")
    {
        std::istringstream iss {"PRINT 3*2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 * 2\n");
    }
    SECTION("recreate a multiply with two double constants")
    {
        std::istringstream iss {"PRINT 3.0*2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 * 2.0\n");
    }
    SECTION("recreate a multiply with one integer and one double constant")
    {
        std::istringstream iss {"PRINT 3*2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 * 2.0\n");
    }
    SECTION("recreate a multiply with one double and one integer constant")
    {
        std::istringstream iss {"PRINT 3.0*2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 * 2\n");
    }
}

TEST_CASE("execute integer-integer multiply operator", "[mul][int-int]")
{
    ProgramUnit program;

    SECTION("execute a multiply of two integers")
    {
        std::istringstream iss {"PRINT 3*2"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "6\n");
    }
    SECTION("execute a multiply that causes an overflow")
    {
        std::istringstream iss {"PRINT 2000000000 * 3"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:17: overflow\n"
            "    PRINT 2000000000 * 3\n"
            "                     ^\n");
    }
    SECTION("execute a multiply that causes an underflow")
    {
        std::istringstream iss {"PRINT 2000000000 * -3"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:17: overflow\n"
            "    PRINT 2000000000 * -3\n"
            "                     ^\n");
    }
}

TEST_CASE("execute double-double multiply operator", "[mul][dbl-dbl]")
{
    ProgramUnit program;

    SECTION("execute a multiply of two doubles")
    {
        std::istringstream iss {"PRINT 3.0*2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "6\n");
    }
    SECTION("execute a multiply that causes an overflow")
    {
        std::istringstream iss {"PRINT 2e300 * 3e20"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:12: overflow\n"
            "    PRINT 2e300 * 3e20\n"
            "                ^\n");
    }
    SECTION("execute a multiply that causes an underflow")
    {
        std::istringstream iss {"PRINT -2e300 * 3e20"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:13: overflow\n"
            "    PRINT -2e300 * 3e20\n"
            "                 ^\n");
    }
    SECTION("check multiplication that results in a negative number")
    {
        std::istringstream iss {"PRINT -2.0 * 3.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() == "-6\n");
    }
}

TEST_CASE("execute double-integer multiply operator", "[mul][dbl-int]")
{
    ProgramUnit program;

    SECTION("execute a multiply of a double and an integer")
    {
        std::istringstream iss {"PRINT 3.0*2"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "6\n");
    }
    SECTION("execute a multiply that causes an overflow")
    {
        std::istringstream iss {"PRINT 2e307 * 30000000"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:12: overflow\n"
            "    PRINT 2e307 * 30000000\n"
            "                ^\n");
    }
}

TEST_CASE("execute integer-double multiply operator", "[mul][int-dbl]")
{
    ProgramUnit program;

    SECTION("execute a multiply of an integer and a double")
    {
        std::istringstream iss {"PRINT 3*2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "6\n");
    }
    SECTION("execute a multiply that causes an overflow")
    {
        std::istringstream iss {"PRINT 20000000 * 3e307"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:15: overflow\n"
            "    PRINT 20000000 * 3e307\n"
            "                   ^\n");
    }
}


TEST_CASE("compile divide operator expressions", "[div][compile]")
{
    ProgramUnit program;

    SECTION("make sure both operands and operator are parsed")
    {
        Compiler compiler {"3/2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate divide operator expressions", "[div][recreate]")
{
    ProgramUnit program;

    SECTION("recreate a divide with two integer constants")
    {
        std::istringstream iss {"PRINT 3/2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 / 2\n");
    }
    SECTION("recreate a divide with two double constants")
    {
        std::istringstream iss {"PRINT 3.0/2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 / 2.0\n");
    }
    SECTION("recreate a divide with one integer and one double constant")
    {
        std::istringstream iss {"PRINT 3/2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 / 2.0\n");
    }
    SECTION("recreate a divide with one double and one integer constant")
    {
        std::istringstream iss {"PRINT 3.0/2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 / 2\n");
    }
}

TEST_CASE("execute integer-integer divide operator", "[div][int-int]")
{
    ProgramUnit program;

    SECTION("execute a divide of two integers")
    {
        std::istringstream iss {"PRINT 3/2"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "1\n");
    }
    SECTION("check for a divide by zero error")
    {
        std::istringstream iss {"PRINT 3/0"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:8: divide by zero\n"
            "    PRINT 3 / 0\n"
            "            ^\n");
    }
}

TEST_CASE("execute double-double divide operator", "[div][dbl-dbl]")
{
    ProgramUnit program;

    SECTION("execute a divide of two doubles")
    {
        std::istringstream iss {"PRINT 3.0/2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "1.5\n");
    }
    SECTION("check for a divide by zero error")
    {
        std::istringstream iss {"PRINT 3.0/0.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:10: divide by zero\n"
            "    PRINT 3.0 / 0.0\n"
            "              ^\n");
    }
    SECTION("execute a divide that causes an overflow")
    {
        std::istringstream iss {"PRINT 2e300 / 3e-20"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:12: overflow\n"
            "    PRINT 2e300 / 3e-20\n"
            "                ^\n");
    }
    SECTION("execute a divide that causes an underflow")
    {
        std::istringstream iss {"PRINT -2e300 / 3e-20"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:13: overflow\n"
            "    PRINT -2e300 / 3e-20\n"
            "                 ^\n");
    }
}

TEST_CASE("execute integer-double divide operator", "[div][int-dbl]")
{
    ProgramUnit program;

    SECTION("execute a divide of an integer by a double")
    {
        std::istringstream iss {"PRINT 3/2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "1.5\n");
    }
    SECTION("check for a divide by zero error")
    {
        std::istringstream iss {"PRINT 3/0.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:8: divide by zero\n"
            "    PRINT 3 / 0.0\n"
            "            ^\n");
    }
    SECTION("execute a divide that causes an overflow")
    {
        std::istringstream iss {"PRINT 200 / 3e-307"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:10: overflow\n"
            "    PRINT 200 / 3e-307\n"
            "              ^\n");
    }
}

TEST_CASE("execute double-integer divide operator", "[div][dbl-int]")
{
    ProgramUnit program;

    SECTION("execute a divide of a double by an integer")
    {
        std::istringstream iss {"PRINT 3.0/2"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "1.5\n");
    }
    SECTION("check for a divide by zero error")
    {
        std::istringstream iss {"PRINT 3.0/0"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:10: divide by zero\n"
            "    PRINT 3.0 / 0\n"
            "              ^\n");
    }
}


TEST_CASE("compile integer divide operator expressions", "[intdiv][compile]")
{
    ProgramUnit program;

    SECTION("make sure both operands and operator are parsed")
    {
        Compiler compiler {"3.0\\2.0", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check for an integer divide code after the operands")
    {
        extern OperatorCode<OpType::DblDbl> int_div_code;

        Compiler compiler {"3.0\\2.0", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == int_div_code.getValue());
        REQUIRE(data_type == DataType::Integer);
    }
    SECTION("make sure multiple integer divide operators are compiled")
    {
        Compiler compiler {"3.0\\2.0\\1.0", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate integer divide operator expressions", "[intdiv][recreate]")
{
    ProgramUnit program;

    SECTION("recreate a divide with two integer constants")
    {
        std::istringstream iss {"PRINT 3.0\\2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 \\ 2.0\n");
    }
}

TEST_CASE("execute integer divide operator", "[intdiv][execute]")
{
    ProgramUnit program;

    SECTION("execute a divide of two integers")
    {
        std::istringstream iss {"PRINT 3.0\\2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "1\n");
    }
    SECTION("check for a divide by zero error")
    {
        std::istringstream iss {"PRINT 3.0\\0.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:10: divide by zero\n"
            "    PRINT 3.0 \\ 0.0\n"
            "              ^\n");
    }
    SECTION("execute a divide that causes an overflow")
    {
        std::istringstream iss {"PRINT 2e20 \\ 2e10"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:11: overflow\n"
            "    PRINT 2e20 \\ 2e10\n"
            "               ^\n");
    }
    SECTION("execute a divide that causes an underflow")
    {
        std::istringstream iss {"PRINT 2e20 \\ -2e10"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:11: overflow\n"
            "    PRINT 2e20 \\ -2e10\n"
            "               ^\n");
    }
}

TEST_CASE("apply necessary conversions to integer divide operator", "[intdiv][conversion]")
{
    ProgramUnit program;

    SECTION("change a left side integer constant to a double constant")
    {
        std::istringstream iss {"PRINT 3\\2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "1\n");
    }
    SECTION("change a right side integer constant to a double constant")
    {
        std::istringstream iss {"PRINT 3.0\\2"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "1\n");
    }
    SECTION("don't change to double constant if last operand was not a constant")
    {
        std::istringstream iss {"PRINT 9.0 / 3.0\\2"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "1\n");
    }
    SECTION("add a convert to double code for a non-constant integer operand")
    {
        extern Code cvtdbl_code;

        Compiler compiler {"3.0\\1*2", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(code_line.size() == 9);
        REQUIRE(code_line[7].instructionCode()->getValue() == cvtdbl_code.getValue());
    }
    SECTION("internal convert to double code functionality")
    {
        std::istringstream iss {"PRINT 3.0\\1*2"};
        std::ostringstream oss;

        program.compile(iss);

        SECTION("check that a convert to double code is recreated to nothing")
        {
            program.recreate(oss);

            REQUIRE(oss.str() == "PRINT 3.0 \\ 1 * 2\n");
        }
        SECTION("execute the convert to double code for a non-constant integer operand")
        {
            program.run(oss);

            REQUIRE(oss.str() == "1\n");
        }
    }
}


TEST_CASE("compile mod operator expressions", "[mod][compile]")
{
    ProgramUnit program;

    SECTION("make sure both operands and operator are parsed")
    {
        Compiler compiler {"5 MOD 3", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() != 'M');
        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check for all of the modulo codes in the compiled code")
    {
        extern OperatorCode<OpType::DblDbl> mod_dbl_dbl_code;
        extern OperatorCode<OpType::IntDbl> mod_int_dbl_code;
        extern OperatorCode<OpType::DblInt> mod_dbl_int_code;
        extern OperatorCode<OpType::IntInt> mod_int_int_code;

        Compiler compiler {"100 MOD 2 Mod 1.5 MOD (5.0 mod 4)", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(code_line.size() == 14);
        REQUIRE(code_line[4].instructionCode()->getValue() == mod_int_int_code.getValue());
        REQUIRE(code_line[7].instructionCode()->getValue() == mod_int_dbl_code.getValue());
        REQUIRE(code_line[12].instructionCode()->getValue() == mod_dbl_int_code.getValue());
        REQUIRE(code_line[13].instructionCode()->getValue() == mod_dbl_dbl_code.getValue());
    }
}

TEST_CASE("recreate mod operator expressions", "[mod][recreate]")
{
    ProgramUnit program;

    SECTION("recreate a mod with two double constants")
    {
        std::istringstream iss {"PRINT 3.0MOD 2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 MOD 2.0\n");
    }
    SECTION("recreate a mod with one integer and one double constant")
    {
        std::istringstream iss {"PRINT 3 Mod2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 MOD 2.0\n");
    }
    SECTION("recreate a mod with one double and one integer constant")
    {
        std::istringstream iss {"PRINT 3.0mod2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 MOD 2\n");
    }
    SECTION("recreate a mod with two integer constants")
    {
        std::istringstream iss {"PRINT 3MOD2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 MOD 2\n");
    }
}

TEST_CASE("execute integer-integer mod operator", "[mod][int-int]")
{
    ProgramUnit program;

    SECTION("execute a mod of two integers")
    {
        std::istringstream iss {"PRINT 5 MOD 3"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "2\n");
    }
    SECTION("check for a divide by zero error")
    {
        std::istringstream iss {"PRINT 5 MOD 0"};
        std::ostringstream oss;

        program.compile(iss);

        SECTION("check that error is thrown")
        {
            REQUIRE_THROWS_AS(program.run(oss), ProgramError);
        }
        SECTION("check for the correct error message, column and length")
        {
            program.runCode(oss);

            REQUIRE(oss.str() ==
                "run error at line 1:8: divide by zero\n"
                "    PRINT 5 MOD 0\n"
                "            ^^^\n");
        }
    }
}

TEST_CASE("execute double-double mod operator", "[mod][dbl-dbl]")
{
    ProgramUnit program;

    SECTION("execute a mod of two doubles")
    {
        std::istringstream iss {"PRINT 5.3 mod 3.1"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "2.2\n");
    }
    SECTION("check for a divide by zero error")
    {
        std::istringstream iss {"PRINT 5.3 mod 0.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:10: divide by zero\n"
            "    PRINT 5.3 MOD 0.0\n"
            "              ^^^\n");
    }
}

TEST_CASE("execute integer-double mod operator", "[mod][int-dbl]")
{
    ProgramUnit program;

    SECTION("execute a mod of an integer by a double")
    {
        std::istringstream iss {"PRINT 5 mod 3.2"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "1.8\n");
    }
    SECTION("check for a divide by zero error")
    {
        std::istringstream iss {"PRINT 5 mod 0.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:8: divide by zero\n"
            "    PRINT 5 MOD 0.0\n"
            "            ^^^\n");
    }
}

TEST_CASE("execute double-integer mod operator", "[mod][dbl-int]")
{
    ProgramUnit program;

    SECTION("execute a mod of a double by an integer")
    {
        std::istringstream iss {"PRINT 5.3 mod 3"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "2.3\n");
    }
    SECTION("check for a divide by zero error")
    {
        std::istringstream iss {"PRINT 5.3 mod 0"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:10: divide by zero\n"
            "    PRINT 5.3 MOD 0\n"
            "              ^^^\n");
    }
}


TEST_CASE("compile add operator expressions", "[add][compile]")
{
    ProgramUnit program;

    SECTION("make sure both operands and operator are parsed")
    {
        Compiler compiler {"3+2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check for all of the add codes in the compiled code")
    {
        extern OperatorCode<OpType::DblDbl> add_dbl_dbl_code;
        extern OperatorCode<OpType::IntDbl> add_int_dbl_code;
        extern OperatorCode<OpType::DblInt> add_dbl_int_code;
        extern OperatorCode<OpType::IntInt> add_int_int_code;

        Compiler compiler {"100 + 2 + 1.5 + (5.0 + 4)", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(code_line.size() == 14);
        REQUIRE(code_line[4].instructionCode()->getValue() == add_int_int_code.getValue());
        REQUIRE(code_line[7].instructionCode()->getValue() == add_int_dbl_code.getValue());
        REQUIRE(code_line[12].instructionCode()->getValue() == add_dbl_int_code.getValue());
        REQUIRE(code_line[13].instructionCode()->getValue() == add_dbl_dbl_code.getValue());
    }
}

TEST_CASE("recreate add operator expressions", "[add][recreate]")
{
    ProgramUnit program;

    SECTION("recreate a add with two double constants")
    {
        std::istringstream iss {"PRINT 3.0+2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 + 2.0\n");
    }
    SECTION("recreate a add with one integer and one double constant")
    {
        std::istringstream iss {"PRINT 3 +2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 + 2.0\n");
    }
    SECTION("recreate a add with one double and one integer constant")
    {
        std::istringstream iss {"PRINT 3.0+2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 + 2\n");
    }
    SECTION("recreate a add with two integer constants")
    {
        std::istringstream iss {"PRINT 3+2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 + 2\n");
    }
}

TEST_CASE("execute integer-integer add operator", "[add][int-int]")
{
    ProgramUnit program;

    SECTION("execute a add of two integers")
    {
        std::istringstream iss {"PRINT 5 + 3"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "8\n");
    }
    SECTION("execute an add that causes an overflow")
    {
        std::istringstream iss {"PRINT 2000000000 + 2000000000"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:17: overflow\n"
            "    PRINT 2000000000 + 2000000000\n"
            "                     ^\n");
    }
    SECTION("execute an add that causes an underflow")
    {
        std::istringstream iss {"PRINT -2000000000 + -2000000000"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:18: overflow\n"
            "    PRINT -2000000000 + -2000000000\n"
            "                      ^\n");
    }
}

TEST_CASE("execute double-double add operator", "[add][dbl-dbl]")
{
    ProgramUnit program;

    SECTION("execute a add of two doubles")
    {
        std::istringstream iss {"PRINT 3.0+2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "5\n");
    }
    SECTION("execute an add that causes an overflow")
    {
        std::istringstream iss {"PRINT 1e308 + 1e308"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:12: overflow\n"
            "    PRINT 1e308 + 1e308\n"
            "                ^\n");
    }
    SECTION("execute an add that causes an underflow")
    {
        std::istringstream iss {"PRINT -1e308 + -1e308"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:13: overflow\n"
            "    PRINT -1e308 + -1e308\n"
            "                 ^\n");
    }
}

TEST_CASE("execute integer-double add operator", "[add][int-dbl]")
{
    ProgramUnit program;

    SECTION("execute a add of an integer and a double")
    {
        std::istringstream iss {"PRINT 3+2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "5\n");
    }
}

TEST_CASE("execute double-integer add operator", "[add][dbl-int]")
{
    ProgramUnit program;

    SECTION("execute a add of a double and an integer")
    {
        std::istringstream iss {"PRINT 3.0+2"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "5\n");
    }
}


TEST_CASE("compile subtract operator expressions", "[sub][compile]")
{
    ProgramUnit program;

    SECTION("make sure both operands and operator are parsed")
    {
        Compiler compiler {"3-2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate subtract operator expressions", "[sub][recreate]")
{
    ProgramUnit program;

    SECTION("recreate a subtract with two integer constants")
    {
        std::istringstream iss {"PRINT 3-2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 - 2\n");
    }
    SECTION("recreate a subtract with two double constants")
    {
        std::istringstream iss {"PRINT 3.0-2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 - 2.0\n");
    }
    SECTION("recreate a subtract with one integer and one double constant")
    {
        std::istringstream iss {"PRINT 3-2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 - 2.0\n");
    }
    SECTION("recreate a subtract with one double and one integer constant")
    {
        std::istringstream iss {"PRINT 3.0-2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 - 2\n");
    }
}

TEST_CASE("execute integer-integer subtract operator", "[sub][int-int]")
{
    ProgramUnit program;

    SECTION("execute a subtract of two integers")
    {
        std::istringstream iss {"PRINT 5 - 3"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "2\n");
    }
    SECTION("execute a subtract causes an overflow")
    {
        std::istringstream iss {"PRINT 2000000000 - -1000000000"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:17: overflow\n"
            "    PRINT 2000000000 - -1000000000\n"
            "                     ^\n");
    }
    SECTION("execute an subtract that causes an underflow")
    {
        std::istringstream iss {"PRINT -2000000000 - 1000000000"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:18: overflow\n"
            "    PRINT -2000000000 - 1000000000\n"
            "                      ^\n");
    }
}

TEST_CASE("execute double-double subtract operator", "[sub][dbl-dbl]")
{
    ProgramUnit program;

    SECTION("execute a subtract of two doubles")
    {
        std::istringstream iss {"PRINT 5.0-2.1"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "2.9\n");
    }
    SECTION("execute an subtract that causes an overflow")
    {
        std::istringstream iss {"PRINT 1e308 - -1e308"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:12: overflow\n"
            "    PRINT 1e308 - -1e308\n"
            "                ^\n");
    }
    SECTION("execute an subtract that causes an underflow")
    {
        std::istringstream iss {"PRINT -1e308 - 1e308"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:13: overflow\n"
            "    PRINT -1e308 - 1e308\n"
            "                 ^\n");
    }
}

TEST_CASE("execute integer-double subtract operator", "[sub][int-dbl]")
{
    ProgramUnit program;

    SECTION("execute a subtract of an integer and a double")
    {
        std::istringstream iss {"PRINT 5-2.1"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "2.9\n");
    }
}

TEST_CASE("execute double-integer subtract operator", "[sub][dbl-int]")
{
    ProgramUnit program;

    SECTION("execute a subtract of a double and an integer")
    {
        std::istringstream iss {"PRINT 5.1-2"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "3.1\n");
    }
}
