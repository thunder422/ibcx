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

TEST_CASE("compile less than operator expressions", "[lt][compile]")
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
    SECTION("check that white space is allowed after the operator")
    {
        Compiler compiler {"3< 2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate less than operator expressions", "[lt][recreate]")
{
    ProgramUnit program;
    std::ostringstream oss;

    SECTION("recreate a less than with two double constants")
    {
        std::istringstream iss {"PRINT 3.0<2.0"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 < 2.0\n");
    }
    SECTION("recreate a less than with one integer and one double constant")
    {
        std::istringstream iss {"PRINT 3 <2.0"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 < 2.0\n");
    }
    SECTION("recreate a less than with one double and one integer constant")
    {
        std::istringstream iss {"PRINT 3.0< 2"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 < 2\n");
    }
    SECTION("recreate a less than with two integer constants")
    {
        std::istringstream iss {"PRINT 3 < 2"};

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 < 2\n");
    }
}

TEST_CASE("execute less than operator", "[lt][execute]")
{
    ProgramUnit program;

    SECTION("execute a double-double less than operator")
    {
        std::istringstream iss {"PRINT 4.0<3.0\nPRINT 3.0<4.0\nPRINT 3.0<3.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "0\n-1\n0\n");
    }
}
