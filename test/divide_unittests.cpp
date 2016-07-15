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

TEST_CASE("compile divide operator expressions", "[compile]")
{
    ProgramUnit program;

    SECTION("make sure both operands and operator are parsed")
    {
        Compiler compiler {"3/2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate divide operator expressions", "[recreate]")
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

TEST_CASE("execute integer-integer divide operator", "[int-int]")
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

TEST_CASE("execute double-double divide operator", "[dbl-dbl]")
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

TEST_CASE("execute integer-double divide operator", "[int-dbl]")
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

TEST_CASE("execute double-integer divide operator", "[dbl-int]")
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
}
