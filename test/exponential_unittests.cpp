/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "compileerror.h"
#include "compiler.h"
#include "operators.h"
#include "programunit.h"


TEST_CASE("compile exponential operator expressions", "[compile]")
{
    ProgramUnit program;

    SECTION("make sure both operands and operator are parsed")
    {
        Compiler compiler {"3^2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("make sure the operator is the correct character")
    {
        Compiler compiler {"3a2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == 'a');
    }
    SECTION("make sure white space is allowed before the operator")
    {
        Compiler compiler {"3 ^2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("make sure white space is allowed after the operator")
    {
        Compiler compiler {"3 ^ 2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("make sure parsing stops if there is no operand")
    {
        Compiler compiler {"^2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == '^');
    }
    SECTION("check for an error if there is a problem with the second operand")
    {
        Compiler compiler {"3^^", program};

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(compiler.compileExpression(DataType::Null), ExpNumExprError);
        }
        SECTION("check the message, column and length of the error thrown")
        {
            try {
                compiler.compileExpression(DataType::Null);
            }
            catch (const ExpNumExprError &error) {
                REQUIRE(error.column == 2);
                REQUIRE(error.length == 1);
            }
        }
    }
    SECTION("check for an integer exponential code at end of code line")
    {
        extern OperatorCode<OpType::IntInt> exp_int_int_code;

        Compiler compiler {"3^2", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Integer);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == exp_int_int_code.getValue());
    }
    SECTION("check for an double exponential code at end of code line")
    {
        extern OperatorCode<OpType::DblDbl> exp_dbl_dbl_code;

        Compiler compiler {"3.0^2.0", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == exp_dbl_dbl_code.getValue());
    }
    SECTION("check for an right side integer exponential code at end of code line")
    {
        extern OperatorCode<OpType::DblInt> exp_dbl_int_code;

        Compiler compiler {"3.0^2", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == exp_dbl_int_code.getValue());
    }
    SECTION("check for an left side integer exponential code at end of code line")
    {
        extern OperatorCode<OpType::IntDbl> exp_int_dbl_code;

        Compiler compiler {"3^2.0", program};
        auto data_type = compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(data_type == DataType::Double);
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == exp_int_dbl_code.getValue());
    }
    SECTION("check that multiple exponential operators are compiled")
    {
        extern OperatorCode<OpType::IntDbl> exp_int_dbl_code;
        extern OperatorCode<OpType::DblInt> exp_dbl_int_code;

        Compiler compiler {"3^2.0^4", program};
        compiler.compileExpression(DataType::Null);
        auto code_line = compiler.getCodeLine();

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}

