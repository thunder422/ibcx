/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "commandcode.h"
#include "commandcompiler.h"
#include "executer.h"
#include "programerror.h"
#include "programunit.h"


TEST_CASE("compile simple PRINT commands", "[compile]")
{
    extern Code const_int_code;
    extern Code print_int_code;
    extern CommandCode print_code;
    extern CommandCode end_code;

    ProgramUnit program;

    SECTION("compile a PRINT command with an expression (single constant for now)")
    {
        auto code_line = CommandCompiler{"PRINT 234", program}();

        REQUIRE(code_line.size() == 4);
        REQUIRE(code_line[0].instructionCode()->getValue() == const_int_code.getValue());
        auto operand = code_line[1].operand();
        REQUIRE(program.constNumDictionary().get(operand) == "234");
        REQUIRE(code_line[2].instructionCode()->getValue() == print_int_code.getValue());
        REQUIRE(code_line[3].instructionCode()->getValue() == print_code.getValue());
    }
    SECTION("compile a PRINT command with an expression (single constant for now)")
    {
        extern Code const_dbl_code;
        extern Code print_dbl_code;

        auto code_line = CommandCompiler{"PRINT -5.6e14", program}();

        REQUIRE(code_line.size() == 4);
        REQUIRE(code_line[0].instructionCode()->getValue() == const_dbl_code.getValue());
        auto operand = code_line[1].operand();
        REQUIRE(program.constNumDictionary().get(operand) == "-5.6e14");
        REQUIRE(code_line[2].instructionCode()->getValue() == print_dbl_code.getValue());
        REQUIRE(code_line[3].instructionCode()->getValue() == print_code.getValue());
    }
    SECTION("compile a lower case PRINT command")
    {
        auto code_line = CommandCompiler{"print", program}();

        REQUIRE(code_line.size() == 1);
        REQUIRE(code_line[0].instructionCode()->getValue() == print_code.getValue());
    }
}

TEST_CASE("execute simple PRINT commands", "[execute]")
{
    ProgramUnit program;

    SECTION("execute a blank PRINT command")
    {
        std::istringstream iss {"PRINT"};
        program.compile(iss);
        std::ostringstream oss;

        auto executer = program.createExecutor(oss);
        executer.executeOneCode();

        REQUIRE(oss.str() == "\n");
    }
    SECTION("execute a PRINT integer constant command")
    {
        std::istringstream iss {"PRINT -1234"};
        program.compile(iss);
        std::ostringstream oss;

        auto executer = program.createExecutor(oss);
        executer.executeOneCode();
        executer.executeOneCode();
        executer.executeOneCode();

        REQUIRE(oss.str() == "-1234\n");
    }
    SECTION("execute a PRINT double constant command")
    {
        std::istringstream iss {"PRINT 23.4e-108"};
        program.compile(iss);
        std::ostringstream oss;

        auto executer = program.createExecutor(oss);
        executer.executeOneCode();
        executer.executeOneCode();
        executer.executeOneCode();

        REQUIRE(oss.str() == "2.34e-107\n");
    }
}
