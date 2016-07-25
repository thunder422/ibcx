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

TEST_CASE("compile subtract operator expressions", "[compile]")
{
    ProgramUnit program;

    SECTION("make sure both operands and operator are parsed")
    {
        Compiler compiler {"3-2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate subtract operator expressions", "[recreate]")
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

TEST_CASE("execute integer-integer subtract operator", "[int-int]")
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

TEST_CASE("execute double-double subtract operator", "[dbl-dbl]")
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
