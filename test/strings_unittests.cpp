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
}
