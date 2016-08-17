/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "compiler.h"
#include "programunit.h"


TEST_CASE("compile absolute function expressions", "[abs][compile]")
{
    ProgramUnit program;

    SECTION("make sure function and argument are parsed")
    {
        Compiler compiler {"ABS(-2)", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check that function code is added after the operand")
    {
        Compiler compiler {"ABS(-2.1)", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        extern Code abs_dbl_code;
        REQUIRE(code_line.size() == 3);
        REQUIRE(code_line[2].instructionCode()->getValue() == abs_dbl_code.getValue());
    }
    SECTION("check for a double return data type")
    {
        Compiler compiler {"ABS(-2.1)", program};
        auto data_type = compiler.compileExpression(DataType::Null);

        REQUIRE(data_type == DataType::Double);
    }
}
