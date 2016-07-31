/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "compiler.h"
#include "operators.h"
#include "programerror.h"
#include "programunit.h"

TEST_CASE("compile less than operator expressions", "[lt][compile]")
{
    ProgramUnit program;

    SECTION("make sure both operands and operator are parsed")
    {
        Compiler compiler {"3<2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check for an less than code after the operands")
    {
        extern OperatorCode<OpType::IntInt> lt_dbl_dbl_code;

        Compiler compiler {"3.0<2.0", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == lt_dbl_dbl_code.getValue());
    }
    SECTION("check for an integer return type for a less than code")
    {
        Compiler compiler {"3.0<2.0", program};
        auto data_type = compiler.compileExpression(DataType::Null);

        REQUIRE(data_type == DataType::Integer);
    }
    SECTION("check that multiple operators are parsed")
    {
        Compiler compiler {"3<2<1", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check that white space is allowed after the operator")
    {
        Compiler compiler {"3< 2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate less than operator expressions", "[lt][recreate]")
{
    ProgramUnit program;
    std::ostringstream oss;

    SECTION("recreate a less than with two double constants")
    {
        std::istringstream iss {"PRINT 3.0<2.0"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 < 2.0\n");
    }
    SECTION("recreate a less than with one integer and one double constant")
    {
        std::istringstream iss {"PRINT 3 <2.0"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 < 2.0\n");
    }
    SECTION("recreate a less than with one double and one integer constant")
    {
        std::istringstream iss {"PRINT 3.0< 2"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 < 2\n");
    }
    SECTION("recreate a less than with two integer constants")
    {
        std::istringstream iss {"PRINT 3 < 2"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 < 2\n");
    }
}

TEST_CASE("execute less than operator", "[lt][execute]")
{
    ProgramUnit program;

    SECTION("execute a double-double less than operator")
    {
        std::istringstream iss {"PRINT 4.0<3.0\nPRINT 3.0<4.0\nPRINT 3.0<3.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0\n-1\n0\n");
    }
    SECTION("execute a integer-double less than operator")
    {
        std::istringstream iss {"PRINT 4<3.0\nPRINT 3<4.0\nPRINT 3<3.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0\n-1\n0\n");
    }
    SECTION("execute a double-integer less than operator")
    {
        std::istringstream iss {"PRINT 4.0<3\nPRINT 3.0<4\nPRINT 3.0<3"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0\n-1\n0\n");
    }
    SECTION("execute a integer-integer less than operator")
    {
        std::istringstream iss {"PRINT 4<3\nPRINT 3<4\nPRINT 3<3"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0\n-1\n0\n");
    }
}


TEST_CASE("compile greater than operator expressions", "[gt][compile]")
{
    ProgramUnit program;

    SECTION("make sure both operands and operator are parsed")
    {
        Compiler compiler {"3>2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate greater than operator expressions", "[gt][recreate]")
{
    ProgramUnit program;
    std::ostringstream oss;

    SECTION("recreate a greater than with two double constants")
    {
        std::istringstream iss {"PRINT 3.0>2.0"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 > 2.0\n");
    }
    SECTION("recreate a greater than with one integer and one double constant")
    {
        std::istringstream iss {"PRINT 3 >2.0"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 > 2.0\n");
    }
    SECTION("recreate a greater than with one double and one integer constant")
    {
        std::istringstream iss {"PRINT 3.0> 2"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 > 2\n");
    }
    SECTION("recreate a greater than with two integer constants")
    {
        std::istringstream iss {"PRINT 3 > 2"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 > 2\n");
    }
}

TEST_CASE("execute greater than operator", "[gt][execute]")
{
    ProgramUnit program;

    SECTION("execute a double-double greater than operator")
    {
        std::istringstream iss {"PRINT 4.0>3.0\nPRINT 3.0>4.0\nPRINT 3.0>3.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-1\n0\n0\n");
    }
    SECTION("execute a integer-double greater than operator")
    {
        std::istringstream iss {"PRINT 4>3.0\nPRINT 3>4.0\nPRINT 3>3.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-1\n0\n0\n");
    }
    SECTION("execute a double-integer greater than operator")
    {
        std::istringstream iss {"PRINT 4.0>3\nPRINT 3.0>4\nPRINT 3.0>3"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-1\n0\n0\n");
    }
    SECTION("execute a integer-integer greater than operator")
    {
        std::istringstream iss {"PRINT 4>3\nPRINT 3>4\nPRINT 3>3"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-1\n0\n0\n");
    }
}


TEST_CASE("compile less than or equal operator expressions", "[le][compile]")
{
    ProgramUnit program;

    SECTION("make sure both operands and operator are parsed")
    {
        Compiler compiler {"3<=2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate less than or equal operator expressions", "[le][recreate]")
{
    ProgramUnit program;
    std::ostringstream oss;

    SECTION("recreate a less than or equal with two double constants")
    {
        std::istringstream iss {"PRINT 3.0<=2.0"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 <= 2.0\n");
    }
    SECTION("recreate a less than or equal with one integer and one double constant")
    {
        std::istringstream iss {"PRINT 3 <=2.0"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 <= 2.0\n");
    }
    SECTION("recreate a less than or equal with one double and one integer constant")
    {
        std::istringstream iss {"PRINT 3.0<= 2"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 <= 2\n");
    }
    SECTION("recreate a less than or equal with two integer constants")
    {
        std::istringstream iss {"PRINT 3 <= 2"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 <= 2\n");
    }
}

TEST_CASE("execute less than or equal operator", "[gt][execute]")
{
    ProgramUnit program;

    SECTION("execute a double-double less than or equal operator")
    {
        std::istringstream iss {"PRINT 4.0<=3.0\nPRINT 3.0<=4.0\nPRINT 3.0<=3.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0\n-1\n-1\n");
    }
    SECTION("execute a integer-double less than or equal operator")
    {
        std::istringstream iss {"PRINT 4<=3.0\nPRINT 3<=4.0\nPRINT 3<=3.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0\n-1\n-1\n");
    }
    SECTION("execute a double-integer less than or equal operator")
    {
        std::istringstream iss {"PRINT 4.0<=3\nPRINT 3.0<=4\nPRINT 3.0<=3"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0\n-1\n-1\n");
    }
    SECTION("execute a integer-integer less than or equal operator")
    {
        std::istringstream iss {"PRINT 4<=3\nPRINT 3<=4\nPRINT 3<=3"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0\n-1\n-1\n");
    }
}


TEST_CASE("compile greater than or equal operator expressions", "[ge][compile]")
{
    ProgramUnit program;

    SECTION("make sure both operands and operator are parsed")
    {
        Compiler compiler {"3>=2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate greater than or equal operator expressions", "[le][recreate]")
{
    ProgramUnit program;
    std::ostringstream oss;

    SECTION("recreate a greater than or equal with two double constants")
    {
        std::istringstream iss {"PRINT 3.0>=2.0"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 >= 2.0\n");
    }
    SECTION("recreate a greater than or equal with one integer and one double constant")
    {
        std::istringstream iss {"PRINT 3 >=2.0"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 >= 2.0\n");
    }
    SECTION("recreate a greater than or equal with one double and one integer constant")
    {
        std::istringstream iss {"PRINT 3.0>= 2"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 >= 2\n");
    }
    SECTION("recreate a greater than or equal with two integer constants")
    {
        std::istringstream iss {"PRINT 3 >= 2"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 >= 2\n");
    }
}

TEST_CASE("execute greater than or equal operator", "[gt][execute]")
{
    ProgramUnit program;

    SECTION("execute a double-double greater than or equal operator")
    {
        std::istringstream iss {"PRINT 4.0>=3.0\nPRINT 3.0>=4.0\nPRINT 3.0>=3.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-1\n0\n-1\n");
    }
    SECTION("execute a integer-double greater than or equal operator")
    {
        std::istringstream iss {"PRINT 4>=3.0\nPRINT 3>=4.0\nPRINT 3>=3.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-1\n0\n-1\n");
    }
    SECTION("execute a double-integer greater than or equal operator")
    {
        std::istringstream iss {"PRINT 4.0>=3\nPRINT 3.0>=4\nPRINT 3.0>=3"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-1\n0\n-1\n");
    }
    SECTION("execute a integer-integer greater than or equal operator")
    {
        std::istringstream iss {"PRINT 4>=3\nPRINT 3>=4\nPRINT 3>=3"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-1\n0\n-1\n");
    }
}


TEST_CASE("compile equal to operator expressions", "[eq][compile]")
{
    ProgramUnit program;

    SECTION("make sure both operands and operator are parsed")
    {
        Compiler compiler {"3=2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("make sure equal operator compiled at different precedence level")
    {
        extern OperatorCode<OpType::IntInt> lt_int_int_code;
        extern OperatorCode<OpType::IntInt> eq_int_int_code;

        Compiler compiler {"0=1<2", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(code_line.size() == 8);
        REQUIRE(code_line[7].instructionCode()->getValue() != lt_int_int_code.getValue());
        REQUIRE(code_line[7].instructionCode()->getValue() == eq_int_int_code.getValue());
    }
    SECTION("check for an integer return type for an equal to code")
    {
        Compiler compiler {"3.0=2.0", program};
        auto data_type = compiler.compileExpression(DataType::Null);

        REQUIRE(data_type == DataType::Integer);
    }
    SECTION("check that multiple operators are parsed")
    {
        Compiler compiler {"3=2=1", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check that white space is allowed after the operator")
    {
        Compiler compiler {"3= 2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate equal to operator expressions", "[eq][recreate]")
{
    ProgramUnit program;
    std::ostringstream oss;

    SECTION("recreate a equal to with two double constants")
    {
        std::istringstream iss {"PRINT 3.0=2.0"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 = 2.0\n");
    }
    SECTION("recreate a equal to with one integer and one double constant")
    {
        std::istringstream iss {"PRINT 3 =2.0"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 = 2.0\n");
    }
    SECTION("recreate a equal to with one double and one integer constant")
    {
        std::istringstream iss {"PRINT 3.0= 2"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 = 2\n");
    }
    SECTION("recreate a equal to with two integer constants")
    {
        std::istringstream iss {"PRINT 3 = 2"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 = 2\n");
    }
}
