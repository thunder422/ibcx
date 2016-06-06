/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "compiler.h"
#include "expressioncompiler.h"
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
        ExpressionCompiler compile_expression {compiler};
        auto data_type = compile_expression(DataType::Null);
        REQUIRE_OPERAND(DataType::Integer, "1");
    }
    SECTION("verify an error is thrown when nothing is in the input stream")
    {
        Compiler compiler {"", program};
        ExpressionCompiler compile_expression {compiler};

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(compile_expression(DataType::Null), CompileError);
        }
        SECTION("check the message, column and length of the error thrown")
        {
            try {
                compile_expression(DataType::Null);
            }
            catch (const CompileError &error) {
                REQUIRE(error.what() == std::string("expected numeric expression"));
                REQUIRE(error.column == 0);
                REQUIRE(error.length == 1);
            }
        }
    }
}
