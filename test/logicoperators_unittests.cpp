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

TEST_CASE("recreate negate operator expressions", "[not][recreate]")
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
}
