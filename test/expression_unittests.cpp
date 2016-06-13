/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "compiler.h"
#include "compileerror.h"
#include "programcode.h"
#include "programunit.h"
#include "support.h"


TEST_CASE("compile numeric expressions with constants", "[constant]")
{
    ProgramUnit program;

    SECTION("compile an expression with an integer constant to the code line")
    {
        extern Code constIntCode;

        Compiler compiler {"1", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        REQUIRE_OPERAND(DataType::Integer, "1");
    }
    SECTION("verify an error is thrown when nothing is in the input stream")
    {
        Compiler compiler {"", program};

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(compiler.compileExpression(DataType::Null), CompileError);
        }
        SECTION("check the message, column and length of the error thrown")
        {
            try {
                compiler.compileExpression(DataType::Null);
            }
            catch (const CompileError &error) {
                REQUIRE(error.what() == std::string("expected numeric expression"));
                REQUIRE(error.column == 0);
                REQUIRE(error.length == 1);
            }
        }
    }
}

TEST_CASE("negate numeric operator", "[negate]")
{
    ProgramUnit program;

    SECTION("compile a negate operator (in front of a negative integer constant)")
    {
        extern Code const_int_code;

        Compiler compiler {"--2", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        REQUIRE(data_type == DataType::Integer);
        REQUIRE_CODESIZE_OPERAND(3, DataType::Integer, "-2");
        REQUIRE(code_line[0].instructionCode()->getValue() == const_int_code.getValue());
    }
    SECTION("compile a negate operator (in front of a negative double constant)")
    {
        extern Code const_dbl_code;

        Compiler compiler {"--2.0", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        REQUIRE(data_type == DataType::Double);
        REQUIRE_CODESIZE_OPERAND(3, DataType::Double, "-2.0");
        REQUIRE(code_line[0].instructionCode()->getValue() == const_dbl_code.getValue());
    }
    SECTION("check that an integer negate code is added to the program (after the constant)")
    {
        extern Code neg_dbl_code;

        Compiler compiler {"--2.0", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(code_line[2].instructionCode()->getValue() == neg_dbl_code.getValue());
    }
}
