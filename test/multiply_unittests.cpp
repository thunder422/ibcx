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

TEST_CASE("compile multiply operator expressions", "[compile]")
{
    ProgramUnit program;

    SECTION("make sure both operands and operator are parsed")
    {
        Compiler compiler {"3*2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check for an integer multiply code after the operands")
    {
        extern OperatorCode<OpType::IntInt> mul_int_int_code;

        Compiler compiler {"3*2", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Integer);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == mul_int_int_code.getValue());
    }
    SECTION("check for an double exponential code after the operands")
    {
        extern OperatorCode<OpType::DblDbl> mul_dbl_dbl_code;

        Compiler compiler {"3.0*2.0", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == mul_dbl_dbl_code.getValue());
    }
    SECTION("check for an right side integer multiply code after the operands")
    {
        extern OperatorCode<OpType::DblInt> mul_dbl_int_code;

        Compiler compiler {"3.0*2", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == mul_dbl_int_code.getValue());
    }
    SECTION("check for an left side integer multiply code after the operands")
    {
        extern OperatorCode<OpType::IntDbl> mul_int_dbl_code;

        Compiler compiler {"3*2.0", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == mul_int_dbl_code.getValue());
    }
    SECTION("check that multiple multiply operators are compiled")
    {
        extern OperatorCode<OpType::IntDbl> mul_int_dbl_code;
        extern OperatorCode<OpType::DblInt> mul_dbl_int_code;

        Compiler compiler {"3*2.0*4", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

TEST_CASE("recreate nultiply operator expression", "[recreate]")
{
    ProgramUnit program;

    SECTION("recreate a multiply with two integer constants")
    {
        std::istringstream iss {"PRINT 3*2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 * 2\n");
    }
    SECTION("recreate a multiply with two double constants")
    {
        std::istringstream iss {"PRINT 3.0*2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 * 2.0\n");
    }
    SECTION("recreate a multiply with one integer and one double constant")
    {
        std::istringstream iss {"PRINT 3*2.0"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3 * 2.0\n");
    }
    SECTION("recreate a multiply with one double and one integer constant")
    {
        std::istringstream iss {"PRINT 3.0*2"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == "PRINT 3.0 * 2\n");
    }
}
