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


TEST_CASE("compile not operator expressions", "[not][compile]")
{
    ProgramUnit program;

    SECTION("make sure operator and operand are parsed")
    {
        Compiler compiler {"NOT-2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check that the not code is added after the operand")
    {
        Compiler compiler {"NOT-2", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        extern Code not_code;
        REQUIRE(code_line.size() == 3);
        REQUIRE(code_line[2].instructionCode()->getValue() == not_code.getValue());
    }
    SECTION("check for an integer return data type")
    {
        Compiler compiler {"NOT 2.0", program};
        auto data_type = compiler.compileExpression(DataType::Null);

        REQUIRE(data_type == DataType::Integer);
    }
    SECTION("check that multiple not operators are parsed")
    {
        Compiler compiler {"NOT not -2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate not operator expressions", "[not][recreate]")
{
    ProgramUnit program;

    SECTION("recreate a not of a negative integer constant")
    {
        std::istringstream iss {"PRINT NOT-2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT NOT -2\n");
    }
    SECTION("recreate a not of a positve integer constant")
    {
        std::istringstream iss {"PRINT NOT 2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT NOT 2\n");
    }
}

TEST_CASE("execute not operator", "[not][execute]")
{
    ProgramUnit program;

    SECTION("execute a not operator")
    {
        std::istringstream iss {"PRINT NOT 2"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-3\n");
    }
}

TEST_CASE("apply necessary conversions to not operator", "[not][conversion]")
{
    ProgramUnit program;

    SECTION("change a double constant to an integer constant")
    {
        std::istringstream iss {"PRINT NOT 2.1"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-3\n");
    }
    SECTION("don't change to an integer constant if operand is not a constant")
    {
        std::istringstream iss {"PRINT NOT 1 + 1"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-3\n");
    }
    SECTION("add a convert to integer code for a non-constant double operand")
    {
        extern Code cvtint_code;

        Compiler compiler {"NOT 1.0 + 1.0", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(code_line.size() == 7);
        REQUIRE(code_line[5].instructionCode()->getValue() == cvtint_code.getValue());
    }
    SECTION("check for a compile error if the double constant cannot be converted to an integer")
    {
        std::istringstream iss {"PRINT NOT 2.1e20"};
        std::ostringstream oss;

        program.compileSource(iss, oss);

        REQUIRE(oss.str() ==
            "error on line 1:10: floating point constant is out of range\n"
            "    PRINT NOT 2.1e20\n"
            "              ^^^^^^\n"
        );
    }
    SECTION("internal convert to integer code functionality")
    {
        std::istringstream iss {"PRINT NOT 1.1 * 2.0"};
        std::ostringstream oss;

        program.compile(iss);

        SECTION("check that a convert to integer code is recreated to nothing")
        {
            program.recreate(oss);

            REQUIRE(oss.str() == "PRINT NOT 1.1 * 2.0\n");
        }
        SECTION("execute the convert to integer code for a non-constant double operand")
        {
            program.run(oss);

            REQUIRE(oss.str() == "-3\n");
        }
    }
    SECTION("check for overflow run error on internal convert to integer code")
    {
        std::istringstream iss {"PRINT NOT 1.1e20 * 2"};
        std::ostringstream oss;

        program.compile(iss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "run error at line 1:10: overflow\n"
            "    PRINT NOT 1.1e20 * 2\n"
            "              ^^^^^^^^^^\n"
        );
    }
}


TEST_CASE("compile and operator expressions", "[and][compile]")
{
    ProgramUnit program;

    SECTION("make sure operator and operand are parsed")
    {
        Compiler compiler {"10 AND 7", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check that the and code is added after the operand")
    {
        Compiler compiler {"10 AND 7", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        extern Code and_code;
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == and_code.getValue());
    }
    SECTION("check for an integer return data type")
    {
        Compiler compiler {"10.0 AND 7", program};
        auto data_type = compiler.compileExpression(DataType::Null);

        REQUIRE(data_type == DataType::Integer);
    }
    SECTION("check that multiple and operators are parsed")
    {
        Compiler compiler {"10 AND 7 AND 28", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate and operator expressions", "[and][recreate]")
{
    ProgramUnit program;

    SECTION("recreate an and of integer constants")
    {
        std::istringstream iss {"PRINT 10 and 7"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 10 AND 7\n");
    }
}

TEST_CASE("execute and operator", "[and][execute]")
{
    ProgramUnit program;

    SECTION("execute a and operator")
    {
        std::istringstream iss {"PRINT 10 AND 7"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "2\n");
    }
}

TEST_CASE("apply necessary conversions to and operator", "[and][conversion]")
{
    ProgramUnit program;

    SECTION("change a left side double constant to an integer constant")
    {
        std::istringstream iss {"PRINT 10.0AND7"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "2\n");
    }
    SECTION("change a right side double constant to an integer constant")
    {
        std::istringstream iss {"PRINT 10AND7.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "2\n");
    }
}
