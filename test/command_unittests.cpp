/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "commandcode.h"
#include "commandcompiler.h"
#include "compiler.h"
#include "compileerror.h"
#include "programcode.h"
#include "programunit.h"


TEST_CASE("compile simple commands", "[simple]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("compile an empty command line")
    {
        std::istringstream iss {""};
        Compiler compiler {iss, code_line, program};
        CommandCompiler compile_command {compiler};
        compile_command();
        REQUIRE(code_line.empty());
    }
    SECTION("compile a simple PRINT command")
    {
        std::istringstream iss {"PRINT"};
        Compiler compiler {iss, code_line, program};
        CommandCompiler compile_command {compiler};
        compile_command();
        REQUIRE(code_line.size() == 1);
        auto code = CommandCode::find("PRINT");
        REQUIRE(code != nullptr);
        REQUIRE(code_line[0].instructionCode() == code->getValue());
    }
    SECTION("compile an END command")
    {
        std::istringstream iss {"END"};
        Compiler compiler {iss, code_line, program};
        CommandCompiler compile_command {compiler};
        compile_command();
        REQUIRE(code_line.size() == 1);
        auto code = CommandCode::find("END");
        REQUIRE(code != nullptr);
        REQUIRE(code_line[0].instructionCode() == code->getValue());
    }
    SECTION("allow white space before a command")
    {
        std::istringstream iss {"   PRINT"};
        Compiler compiler {iss, code_line, program};
        CommandCompiler compile_command {compiler};
        compile_command();
        REQUIRE(code_line.size() == 1);
        auto code = CommandCode::find("PRINT");
        REQUIRE(code_line[0].instructionCode() == code->getValue());
    }
    SECTION("check for an error if non-alphabetic word if first")
    {
        std::istringstream iss {"   123"};
        Compiler compiler {iss, code_line, program};
        CommandCompiler compile_command {compiler};
        REQUIRE_THROWS_AS(compile_command(), CompileError);
    }
    SECTION("compile a PRINT comamnd with an expression (single constant for now)")
    {
        extern Code const_int_code;
        extern Code print_int_code;

        std::istringstream iss {"PRINT 234"};
        Compiler compiler {iss, code_line, program};
        CommandCompiler compile_command {compiler};
        compile_command();
        REQUIRE(code_line.size() == 4);
        REQUIRE(code_line[0].instructionCode() == const_int_code.getValue());
        auto operand = code_line[1].operand();
        REQUIRE(program.constIntDictionary().get(operand) == "234");
        REQUIRE(code_line[2].instructionCode() == print_int_code.getValue());
        auto code = CommandCode::find("PRINT");
        REQUIRE(code_line[3].instructionCode() == code->getValue());
    }
    SECTION("compile a PRINT comamnd with an expression (single constant for now)")
    {
        extern Code const_dbl_code;
        extern Code print_dbl_code;

        std::istringstream iss {"PRINT -5.6e14"};
        Compiler compiler {iss, code_line, program};
        CommandCompiler compile_command {compiler};
        compile_command();
        REQUIRE(code_line.size() == 4);
        REQUIRE(code_line[0].instructionCode() == const_dbl_code.getValue());
        auto operand = code_line[1].operand();
        REQUIRE(program.constDblDictionary().get(operand) == "-5.6e14");
        REQUIRE(code_line[2].instructionCode() == print_dbl_code.getValue());
        auto code = CommandCode::find("PRINT");
        REQUIRE(code_line[3].instructionCode() == code->getValue());
    }
}
