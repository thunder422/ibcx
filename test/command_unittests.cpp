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
#include "executer.h"
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
        SECTION("check the message, column and length of the error thrown")
        {
            try {
                compile_command();
            }
            catch (const CompileError &error) {
                std::string expected = "expected command keyword";
                REQUIRE(error.what() == expected);
                REQUIRE(error.column == 3);
                REQUIRE(error.length == 1);
            }
        }
    }
    SECTION("verify error column when constant is not at the beginning of the stream")
    {
        CommandCompiler compile_command {"print 01", program};

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(compile_command(), CompileError);
        }
        SECTION("check the message, column and length of the error thrown")
        {
            try {
                compile_command();
            }
            catch (const CompileError &error) {
                std::string expected = "expected decimal point after leading zero";
                REQUIRE(error.what() == expected);
                REQUIRE(error.column == 7);
                REQUIRE(error.length == 1);
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

        SECTION("recreate program")
        {
            REQUIRE(program.recreateLine(0) == "PRINT -2.45");
            REQUIRE(program.recreateLine(1) == "PRINT");
            REQUIRE(program.recreateLine(2) == "PRINT 123");
            REQUIRE(program.recreateLine(3) == "END");
        }
        SECTION("execute program")
        {
            std::ostringstream oss;

            program.run(oss);
            REQUIRE(oss.str() ==
                "-2.45\n"
                "\n"
                "123\n"
            );
        }
    }
    SECTION("program with no END")
    {
        std::istringstream iss(
            "print 1.704e23\n"
            "print -87654321\n"
        );

        REQUIRE(program.compileSource(iss));

        std::ostringstream oss;
        program.run(oss);

        REQUIRE(oss.str() ==
            "1.704e+23\n"
            "-87654321\n"
        );

        oss.str("");
        program.recreate(oss);

        REQUIRE(oss.str() ==
            "PRINT 1.704e23\n"
            "PRINT -87654321\n"
        );
    }
    SECTION("program with errors")
    {
        std::istringstream iss(
            "print 1.704e%23\n"
            "print 2.45e3000\n"
        );

        REQUIRE_FALSE(program.compileSource(iss));
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
    SECTION("check the message, column and length of the error thrown")
    {
        try {
            compile_command();
        }
        catch (const CompileError &error) {
            std::string expected = "floating point constant is out of range";
            REQUIRE(error.what() == expected);
            REQUIRE(error.column == 6);
            REQUIRE(error.length == 9);
        }
    }
}

TEST_CASE("execute an END command", "[END]")
{
    std::istringstream iss("END");
    ProgramUnit program;
    program.compileSource(iss);
    std::ostringstream not_used_oss;

    auto executer = program.createExecutor(not_used_oss);
    REQUIRE_THROWS_AS(executer.executeOneCode(), EndOfProgram);
}
