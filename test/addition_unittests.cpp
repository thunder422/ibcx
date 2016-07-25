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

TEST_CASE("compile add operator expressions", "[compile]")
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

TEST_CASE("recreate add operator expressions", "[recreate]")
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

TEST_CASE("execute integer-integer add operator", "[int-int]")
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

TEST_CASE("execute double-double add operator", "[dbl-dbl]")
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

TEST_CASE("execute integer-double add operator", "[int-dbl]")
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

TEST_CASE("execute double-integer add operator", "[dbl-int]")
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
