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


TEST_CASE("compile simple commands", "[compile]")
{
    extern Code const_int_code;
    extern Code print_int_code;
    extern CommandCode print_code;
    extern CommandCode end_code;

    ProgramUnit program;

    SECTION("compile an empty command line")
    {
        auto code_line = CommandCompiler{"", program}();

        REQUIRE(code_line.empty());
    }
    SECTION("compile a simple PRINT command")
    {
        auto code_line = CommandCompiler{"PRINT", program}();

        REQUIRE(code_line.size() == 1);
        REQUIRE(code_line[0].instructionCode()->getValue() == print_code.getValue());
    }
    SECTION("compile an END command")
    {
        auto code_line = CommandCompiler{"END", program}();

        REQUIRE(code_line.size() == 1);
        REQUIRE(code_line[0].instructionCode()->getValue() == end_code.getValue());
    }
    SECTION("allow white space before a command")
    {
        auto code_line = CommandCompiler{"   PRINT", program}();

        REQUIRE(code_line.size() == 1);
        REQUIRE(code_line[0].instructionCode()->getValue() == print_code.getValue());
    }
    SECTION("check for an error if non-alphabetic word if first")
    {
        CommandCompiler compile_command {"   123", program};

        SECTION("check that error is thrown")
        {
            REQUIRE_THROWS_AS(compile_command(), CompileError);
        }
        SECTION("check the message and column of the error thrown")
        {
            try {
                compile_command();
            }
            catch (const CompileError &error) {
                std::string expected = "expected command keyword";
                REQUIRE(error.what() == expected);
                REQUIRE(error.column == 3);
            }
        }
    }
    SECTION("compile a PRINT comamnd with an expression (single constant for now)")
    {
        auto code_line = CommandCompiler{"PRINT 234", program}();

        REQUIRE(code_line.size() == 4);
        REQUIRE(code_line[0].instructionCode()->getValue() == const_int_code.getValue());
        auto operand = code_line[1].operand();
        REQUIRE(program.constNumDictionary().get(operand) == "234");
        REQUIRE(code_line[2].instructionCode()->getValue() == print_int_code.getValue());
        REQUIRE(code_line[3].instructionCode()->getValue() == print_code.getValue());
    }
    SECTION("compile a PRINT comamnd with an expression (single constant for now)")
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
    SECTION("verify error column when constant is not at the beginning of the stream")
    {
        CommandCompiler compile_command {"print 01", program};

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(compile_command(), CompileError);
        }
        SECTION("check the message and column of the error thrown")
        {
            try {
                compile_command();
            }
            catch (const CompileError &error) {
                std::string expected = "expected decimal point after leading zero";
                REQUIRE(error.what() == expected);
                REQUIRE(error.column == 7);
            }
        }
    }
}

TEST_CASE("recreate simple commands", "[recreate]")
{
    extern Code const_dbl_code;
    extern Code print_dbl_code;
    extern CommandCode print_code;
    extern CommandCode end_code;

    ProgramUnit program;
    Compiler compiler {"", program};

    SECTION("recreate a blank PRINT command")
    {
        compiler.addInstruction(print_code);
        auto code_line = compiler.getCodeLine();
        program.appendCodeLine(code_line);

        REQUIRE(program.recreateLine(0) == "PRINT");
    }
    SECTION("recreate an END command")
    {
        compiler.addInstruction(end_code);
        auto code_line = compiler.getCodeLine();
        program.appendCodeLine(code_line);

        REQUIRE(program.recreateLine(0) == "END");
    }
    SECTION("recreate a PRINT command with an expression (single constant)")
    {
        compiler.addConstNumInstruction(true, "-1.23e45");
        compiler.addInstruction(print_dbl_code);
        compiler.addInstruction(print_code);
        auto code_line = compiler.getCodeLine();
        program.appendCodeLine(code_line);

        auto source_line = program.recreateLine(0);
        REQUIRE(source_line == "PRINT -1.23e45");
    }
}

TEST_CASE("compile mulitple line program", "[program]")
{
    ProgramUnit program;

    SECTION("program with END")
    {
        std::istringstream iss(
            "PRINT -2.45\n"
            "Print\n"
            "print 123\n"
            "END\n"
        );

        program.compileSource(iss);

        REQUIRE(program.recreateLine(0) == "PRINT -2.45");
        REQUIRE(program.recreateLine(1) == "PRINT");
        REQUIRE(program.recreateLine(2) == "PRINT 123");
        REQUIRE(program.recreateLine(3) == "END");
    }
}

TEST_CASE("correct error column on large double constant", "[large-constant]")
{
    ProgramUnit program;

    CommandCompiler compile_command {"PRINT 1.23e4567", program};

    SECTION("check that error is thrown")
    {
        REQUIRE_THROWS_AS(compile_command(), CompileError);
    }
    SECTION("check the message and column of the error thrown")
    {
        try {
            compile_command();
        }
        catch (const CompileError &error) {
            std::string expected = "floating point constant is out of range";
            REQUIRE(error.what() == expected);
            REQUIRE(error.column == 6);
        }
    }
}
