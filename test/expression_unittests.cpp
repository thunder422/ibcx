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

TEST_CASE("compile expressions with the multiply operator", "[multiply]")
{
    ProgramUnit program;

    SECTION("recreate lower precedence multiply without parentheses with negate operator")
    {
        std::istringstream iss {"PRINT 3*- 2*4"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 * - 2 * 4\n");
    }
    SECTION("recreate parentheses with multiply that is lower precedence than negate operator")
    {
        std::istringstream iss {"PRINT -(2*4)"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT -(2 * 4)\n");
    }
}

TEST_CASE("check expressions with various operators", "[operator]")
{
    ProgramUnit program;

    SECTION("recreate higher precedence unary negate with addition")
    {
        std::istringstream iss {"PRINT - 2+4"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);
        program.run(oss);

        REQUIRE(oss.str() == "PRINT - 2 + 4\n2\n");
    }
    SECTION("check parsing of unary not operator on a right side operand")
    {
        std::istringstream iss {"PRINT 2 + NOT-4"};
        std::ostringstream oss;

        if (program.compileSource(iss, oss)) {
            program.recreate(oss);
            program.run(oss);
        }

        REQUIRE(oss.str() == "PRINT 2 + NOT -4\n5\n");
    }
    SECTION("make sure compiler column is correct when parsing a word")
    {
        Compiler compiler {"3+2bad", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.getColumn() == 3);
    }
    SECTION("make sure eqv operator is parsed correctly when there are no spaces")
    {
        Compiler compiler {"3+2eqv5", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("mark sure compiler column is correct when parsing a word beginning with 'E'")
    {
        Compiler compiler {"3+2ebad", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.getColumn() == 3);
    }
}
