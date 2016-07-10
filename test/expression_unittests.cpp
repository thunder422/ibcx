/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "compileerror.h"
#include "compiler.h"
#include "programerror.h"
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
            REQUIRE_THROWS_AS(compiler.compileExpression(DataType::Double), CompileError);
        }
        SECTION("check the message, column and length of the error thrown")
        {
            try {
                compiler.compileExpression(DataType::Double);
            }
            catch (const CompileError &error) {
                REQUIRE(error.what() == std::string("expected numeric expression"));
                REQUIRE(error.column == 0);
                REQUIRE(error.length == 1);
            }
        }
    }
}

TEST_CASE("compile numeric expressions with parentheses", "[parentheses]")
{
    ProgramUnit program;

    SECTION("verify that a parenthetical expression is compiled")
    {
        Compiler compiler {"4^(3^2)", program};
        compiler.compileExpression(DataType::Null);

        SECTION("verify that opening paretheses is accepted")
        {
            REQUIRE(compiler.peekNextChar() != '(');
        }
        SECTION("verify that the expression inside the parentheses is compiled")
        {
            auto code_line = compiler.getCodeLine();
            REQUIRE(code_line.size() == 8);
        }
        SECTION("verify that the closing parenses is accepted")
        {
            REQUIRE(compiler.peekNextChar() == EOF);
        }
    }
    SECTION("verify that white space is allowed after the opening parenthesis")
    {
        Compiler compiler {"4^( 3^2)", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("detect missing closing parenthesis")
    {
        Compiler compiler {"4^(3^2", program};

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(compiler.compileExpression(DataType::Null), CompileError);
        }
        SECTION("check the message, column and length of the error thrown")
        {
            try {
                compiler.compileExpression(DataType::Double);
            }
            catch (const CompileError &error) {
                REQUIRE(error.what() == std::string("expected closing parentheses"));
                REQUIRE(error.column == 6);
                REQUIRE(error.length == 1);
            }
        }
    }
    SECTION("recreate parentheses around the left side operand")
    {
        std::istringstream iss {"PRINT (- 3)^2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT (- 3) ^ 2\n");
    }
    SECTION("recreate parentheses around the right side operand")
    {
        std::istringstream iss {"PRINT 4^(3^2)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 4 ^ (3 ^ 2)\n");
    }
    SECTION("don't recreate parentheses around right side if there is a unary operator")
    {
        std::istringstream iss {"PRINT 3^- 1^2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 ^ - 1 ^ 2\n");
    }
    SECTION("recreate parentheses around left side containing a unary operator")
    {
        std::istringstream iss {"PRINT (3^- 1)^2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT (3 ^ - 1) ^ 2\n");
    }
    SECTION("don't recreate extra parentheses around left side containing a unary operator")
    {
        std::istringstream iss {"PRINT (- 3)^1^2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT (- 3) ^ 1 ^ 2\n");
    }
}
