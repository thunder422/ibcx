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

TEST_CASE("compile divide operator expressions", "[compile]")
{
    ProgramUnit program;

    SECTION("make sure both operands and operator are parsed")
    {
        Compiler compiler {"3/2", program};
        compiler.compileExpression(DataType::Null);

        REQUIRE(compiler.peekNextChar() == EOF);
    }
}
