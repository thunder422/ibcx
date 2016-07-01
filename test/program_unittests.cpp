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
#include "programerror.h"
#include "programunit.h"
#include "runerror.h"


TEST_CASE("compile simple commands", "[commands]")
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

TEST_CASE("compile multiple line program", "[compile]")
{
    ProgramUnit program;

    SECTION("program with END")
    {
        std::istringstream iss {
            "PRINT -2.45\n"
            "Print\n"
            "print 123\n"
            "END\n"
        };

        program.compile(iss);

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
        std::istringstream iss {
            "print 1.704e123\n"
            "print -87654321\n"
        };

        REQUIRE(program.compile(iss).empty());

        std::ostringstream oss;
        program.run(oss);

        REQUIRE(oss.str() ==
            "1.704e+123\n"
            "-87654321\n"
        );

        oss.str("");
        program.recreate(oss);

        REQUIRE(oss.str() ==
            "PRINT 1.704e123\n"
            "PRINT -87654321\n"
        );
    }
    SECTION("program with errors")
    {
        std::istringstream iss {
            "print 1.704e%23\n"
            "print 2.45e3000\n"
        };
        std::ostringstream oss;

        REQUIRE_FALSE(program.compileSource(iss, oss));

        REQUIRE(oss.str() ==
            "error on line 1:12: expected sign or digit for exponent\n"
            "    print 1.704e%23\n"
            "                ^\n"
            "error on line 2:6: floating point constant is out of range\n"
            "    print 2.45e3000\n"
            "          ^^^^^^^^^\n"
        );
    }
    SECTION("program with stack not empty at end bug (simulated for this test)")
    {
        Compiler compiler {"", program};
        compiler.addConstNumInstruction(true, "-1.23e45");
        auto code_line = compiler.getCodeLine();
        program.appendCodeLine(code_line);

        std::ostringstream oss;
        SECTION("check that error is thrown")
        {
            REQUIRE_THROWS_AS(program.run(oss), ProgramError);
        }
        SECTION("check message of the error thrown")
        {
            try {
                program.run(oss);
            }
            catch (const ProgramError &error) {
                std::string expected = "BUG: value stack not empty at end of program";
                REQUIRE(error.what() == expected);
                REQUIRE(error.line.empty());
            }
        }
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
    std::istringstream iss {"END"};
    ProgramUnit program;
    std::ostringstream unused_oss;
    program.compile(iss);

    auto executer = program.createExecutor(unused_oss);
    REQUIRE_THROWS_AS(executer.executeOneCode(), EndOfProgram);
}

TEST_CASE("run program code", "[execute]")
{
    ProgramUnit program;
    std::ostringstream oss;

    SECTION("run error occurring at a line other than the first")
    {
        std::istringstream iss {
            "PRINT 2^3^4\n"
            "PRINT 0^4^-1\n"
        };
        program.compile(iss);

        REQUIRE_FALSE(program.runCode(oss));
        REQUIRE(oss.str() ==
            "4096\n"
            "run error at line 2:12: divide by zero\n"
            "    PRINT 0 ^ 4 ^ -1\n"
            "                ^\n");
    }
    SECTION("contrived program with stack run error (test bug output)")
    {
        Compiler compiler {"", program};
        compiler.addConstNumInstruction(true, "-1.23e45");
        auto code_line = compiler.getCodeLine();
        program.appendCodeLine(code_line);

        REQUIRE_FALSE(program.runCode(oss));
        REQUIRE(oss.str() ==
            "run error at end of program: BUG: value stack not empty at end of program\n");
    }
    SECTION("successful run of program")
    {
        std::istringstream iss {
            "PRINT 2^3^4\n"
        };
        program.compile(iss);

        REQUIRE(program.runCode(oss));
        REQUIRE(oss.str() ==
            "4096\n");
    }
}

TEST_CASE("miscellaneous error class coverage", "[misc-coverage]")
{
    SECTION("cover dynamically allocated compile error class")
    {
        std::unique_ptr<CompileError> error {new CompileError {"cover dynamic destructor", 0, 0}};
    }
    SECTION("cover dynamically allocated run error class")
    {
        std::unique_ptr<RunError> error {new RunError {"cover dynamic destructor", 0}};
    }
    SECTION("cover dynamically allocated program error class")
    {
        RunError run_error {"cover dynamic destructor", 0};
        std::unique_ptr<ProgramError> error {new ProgramError {run_error}};
    }
    SECTION("cover dynamically allocated expected numeric expression error class")
    {
        std::unique_ptr<ExpNumExprError> error {new ExpNumExprError {0}};
    }
}
