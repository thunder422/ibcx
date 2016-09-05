/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "compiler.h"
#include "programunit.h"


TEST_CASE("string constants", "[const][compile]")
{
    ProgramUnit program;

    SECTION("parse the initial double quote character")
    {
        Compiler compiler {R"*("test")*", program};

        compiler.compileStringConstant();
        REQUIRE(compiler.peekNextChar() != '"');
    }
    SECTION("parse past the ending double quote character")
    {
        Compiler compiler {R"*("test")*", program};

        compiler.compileStringConstant();
        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("make sure parsing stops if first character is not a double quote")
    {
        Compiler compiler {"123", program};

        compiler.compileStringConstant();
        REQUIRE(compiler.peekNextChar() == '1');
    }
    SECTION("make sure parsing stops after ending double quote")
    {
        Compiler compiler {R"*("test"+)*", program};

        compiler.compileStringConstant();
        REQUIRE(compiler.peekNextChar() == '+');
    }
    SECTION("parse two sequential double quotes as part of the constant")
    {
        Compiler compiler {R"*("te""st")*", program};

        compiler.compileStringConstant();
        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check for a null return data type when there is no string constant")
    {
        Compiler compiler {"123", program};

        auto data_type = compiler.compileStringConstant();
        REQUIRE(data_type == DataType::Null);
    }
    SECTION("check for a string return data type when there is a string constant")
    {
        Compiler compiler {R"*("test")*", program};

        auto data_type = compiler.compileStringConstant();
        REQUIRE(data_type == DataType::String);
    }
    SECTION("check that a string constant is added to the program code")
    {
        Compiler compiler {R"*("test")*", program};

        compiler.compileStringConstant();
        auto code_line = compiler.getCodeLine();

        extern Code const_str_code;
        REQUIRE(code_line.size() == 2);
        REQUIRE(code_line[0].instructionCode()->getValue() == const_str_code.getValue());
    }
    SECTION("check that each string has a unique operand (index) value")
    {
        Compiler compiler1 {R"*("first")*", program};
        compiler1.compileStringConstant();
        Compiler compiler2 {R"*("second")*", program};
        compiler2.compileStringConstant();

        auto code_line1 = compiler1.getCodeLine();
        auto code_line2 = compiler2.getCodeLine();

        REQUIRE(code_line1.size() == 2);
        REQUIRE(code_line2.size() == 2);
        REQUIRE(code_line1[1].operand() == 0);
        REQUIRE(code_line2[1].operand() == 1);
    }
}
