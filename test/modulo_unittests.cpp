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

TEST_CASE("compile mod operator expressions", "[compile]")
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

TEST_CASE("recreate mod operator expressions", "[recreate]")
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

TEST_CASE("execute integer-integer mod operator", "[int-int]")
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

TEST_CASE("execute double-double mod operator", "[dbl-dbl]")
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

TEST_CASE("execute integer-double mod operator", "[int-dbl]")
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

TEST_CASE("execute double-integer mod operator", "[dbl-int]")
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