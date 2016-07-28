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

TEST_CASE("compile less than operator expressions", "[compile]")
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
}

