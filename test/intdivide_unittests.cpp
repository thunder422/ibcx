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

TEST_CASE("compile integer divide operator expressions", "[compile]")
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
}

TEST_CASE("recreate integer divide operator expressions", "[recreate]")
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

TEST_CASE("execute integer divide operator", "[execute]")
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

TEST_CASE("apply necessary conversions to integer divide operator", "[conversion]")
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
}
