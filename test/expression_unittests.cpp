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
    ProgramCode code_line;

    SECTION("compile an expression with an integer constant to the code line")
    {
        extern Code constIntCode;

        std::istringstream iss {"1"};
        Compiler compiler {iss, code_line, program};
        ExpressionCompiler compile_expression {compiler};
        auto data_type = compile_expression(DataType::Null);
        REQUIRE_INTEGER_OPERAND("1");
    }
    SECTION("verify an error is thrown when nothing is in the input stream")
    {
        std::istringstream iss;
        Compiler compiler {iss, code_line, program};
        ExpressionCompiler compile_expression {compiler};
        REQUIRE_THROWS_AS(compile_expression(DataType::Null), CompileError);
    }
    SECTION("verify error message and column when nothing is left in the input stream")
    {
        std::istringstream iss {"word "};
        std::string skip_word;
        iss >> skip_word;
        auto skip_space = iss.get();
        Compiler compiler {iss, code_line, program};
        ExpressionCompiler compile_expression {compiler};
        try {
            compile_expression(DataType::Null);
        }
        catch (const CompileError &error) {
            REQUIRE(error.what() == std::string("expected numeric expression"));
            REQUIRE(error.column == 5);
        }
    }
}
