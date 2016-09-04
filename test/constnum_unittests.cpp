/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "compiler.h"
#include "constnumcompiler.h"
#include "compileerror.h"
#include "executer.h"
#include "programcode.h"
#include "programerror.h"
#include "programunit.h"
#include "support.h"


TEST_CASE("compiling integer constants from a string", "[integers]")
{
    ProgramUnit program;

    SECTION("input stream does not contain a constant (caller will determine action)")
    {
        Compiler compiler {"%", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        auto code_line = compiler.getCodeLine();
        REQUIRE(data_type == DataType::Null);
        REQUIRE(code_line.size() == 0);
        REQUIRE(compiler.peekNextChar() == '%');
    }
    SECTION("compile a single digit number")
    {
        extern Code const_int_code;

        Compiler compiler {"1", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Integer, "1");
        REQUIRE(code_line[0].instructionCode()->getValue() == const_int_code.getValue());
    }
    SECTION("compile a multiple digit number")
    {
        Compiler compiler {"123", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Integer, "123");
    }
    SECTION("compile a negative number")
    {
        Compiler compiler {"-234", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Integer, "-234");
    }
    SECTION("terminate compiling at correct character")
    {
        Compiler compiler {"345+", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Integer, "345");
        REQUIRE(compiler.peekNextChar() == '+');
    }
}


TEST_CASE("compiling floating point constants from a string", "[doubles]")
{
    ProgramUnit program;

    SECTION("compile a number with a decimal point")
    {
        Compiler compiler {"0.5", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Double, "0.5");
    }
    SECTION("compile a number with a decimal point at the beginning")
    {
        Compiler compiler {".75", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Double, ".75");
    }
    SECTION("compile a number with a second decimal point (should ignore second one)")
    {
        Compiler compiler {"0.1.", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Double, "0.1");
        REQUIRE(compiler.peekNextChar() == '.');
    }
    SECTION("compile a number to a double constant in the code_line")
    {
        extern Code const_dbl_code;

        Compiler compiler {"1.2", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Double, "1.2");
        REQUIRE(code_line[0].instructionCode()->getValue() == const_dbl_code.getValue());
    }
    SECTION("compile a number with an exponent")
    {
        Compiler compiler {"1e0", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Double, "1e0");
    }
    SECTION("make sure compiling stops before a second 'E'")
    {
        Compiler compiler {"1e0E", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Double, "1e0");
        REQUIRE(compiler.peekNextChar() == 'E');
    }
    SECTION("compile a number with a minus exponent")
    {
        Compiler compiler {"1e-2", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Double, "1e-2");
    }
    SECTION("compile a number with a minus exponent terminated be a minus operator")
    {
        Compiler compiler {"1e-2-", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Double, "1e-2");
        REQUIRE(compiler.peekNextChar() == '-');
    }
    SECTION("compile a number with a plus exponent")
    {
        Compiler compiler {"1e+2", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Double, "1e+2");
    }
}


TEST_CASE("handle leading zero of a constant correctly including errors", "[zero]")
{
    ProgramUnit program;

    SECTION("check for an error when a leading zero is not followed by a digit")
    {
        Compiler compiler {"01", program};
        auto constant_number = ConstNumCompiler::create(compiler);

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(constant_number->compile(), CompileError);
        }
        SECTION("check the message, column and length of the error thrown")
        {
            try {
                constant_number->compile();
            }
            catch (const CompileError &error) {
                std::string expected = "expected decimal point after leading zero";
                REQUIRE(error.what() == expected);
                REQUIRE(error.column == 1);
                REQUIRE(error.length == 1);
            }
        }
    }
    SECTION("check compiling ends when followed by a non-period non-digit")
    {
        Compiler compiler {"0-", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Integer, "0");
        REQUIRE(compiler.peekNextChar() == '-');
    }
}


TEST_CASE("handle leading period of a constant correctly including errors", "[period]")
{
    ProgramUnit program;

    SECTION("check for an error when a leading period is followed by another period")
    {
        Compiler compiler {"..", program};
        auto constant_number = ConstNumCompiler::create(compiler);

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(constant_number->compile(), CompileError);
        }
        SECTION("check the message, column and length of the error thrown")
        {
            try {
                constant_number->compile();
            }
            catch (const CompileError &error) {
                std::string expected = "expected digit after decimal point";
                REQUIRE(error.what() == expected);
                REQUIRE(error.column == 1);
                REQUIRE(error.length == 1);
            }
        }
    }
}


TEST_CASE("check for correct exponent format", "[exponent]")
{
    ProgramUnit program;

    SECTION("check for an error if no sign or digits at the start of an exponent")
    {
        Compiler compiler {"1e.", program};
        auto constant_number = ConstNumCompiler::create(compiler);

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(constant_number->compile(), CompileError);
        }
        SECTION("check the message, column and length of the error thrown")
        {
            try {
                constant_number->compile();
            }
            catch (const CompileError &error) {
                std::string expected = "expected sign or digit for exponent";
                REQUIRE(error.what() == expected);
                REQUIRE(error.column == 2);
                REQUIRE(error.length == 1);
            }
        }
    }
    SECTION("allow for possible EQV operator ('E' lost, which will be handled by caller)")
    {
        Compiler compiler {"1eq", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Integer, "1");
        REQUIRE(compiler.peekNextChar() == 'q');
    }
    SECTION("check for an error if there is no digit after an exponent sign")
    {
        Compiler compiler {"1e-A", program};
        auto constant_number = ConstNumCompiler::create(compiler);

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(constant_number->compile(), CompileError);
        }
        SECTION("check the message, column and length of the error thrown")
        {
            try {
                constant_number->compile();
            }
            catch (const CompileError &error) {
                std::string expected = "expected digit after exponent sign";
                REQUIRE(error.what() == expected);
                REQUIRE(error.column == 3);
                REQUIRE(error.length == 1);
            }
        }
    }
    SECTION("check for an error if terminated by end of the line after an exponent sign")
    {
        Compiler compiler {"1e+", program};
        auto constant_number = ConstNumCompiler::create(compiler);

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(constant_number->compile(), CompileError);
        }
        SECTION("check the message, column and length of the error thrown")
        {
            try {
                constant_number->compile();
            }
            catch (const CompileError &error) {
                REQUIRE(error.column == 3);
                REQUIRE(error.length == 1);
            }
        }
    }
}


TEST_CASE("look for possible exit conditions", "[exit]")
{
    ProgramUnit program;

    SECTION("look for possible negate operator ('-' not followed by '.' or digit)")
    {
        Compiler compiler {"-e", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        auto code_line = compiler.getCodeLine();
        REQUIRE(data_type == DataType::Null);
        REQUIRE(code_line.size() == 0);
    }
    SECTION("allow a period after a negative sign")
    {
        Compiler compiler {"-.1", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Double, "-.1");
    }
    SECTION("look for negate operator status (false if not negate operator)")
    {
        Compiler compiler {"-1-", program};
        auto constant_number = ConstNumCompiler::create(compiler);
        auto data_type = constant_number->compile();
        REQUIRE_OPERAND(DataType::Integer, "-1");
        REQUIRE(compiler.peekNextChar() == '-');
        REQUIRE_FALSE(constant_number->negateOperator());
    }
    SECTION("look for negate operator status (true for negate operator)")
    {
        Compiler compiler {"-e", program};
        auto constant_number = ConstNumCompiler::create(compiler);
        auto data_type = constant_number->compile();
        auto code_line = compiler.getCodeLine();
        REQUIRE(data_type == DataType::Null);
        REQUIRE(compiler.peekNextChar() == 'e');
        REQUIRE(constant_number->negateOperator());
    }
    SECTION("look for possible operator status (false if no operator starting with 'E')")
    {
        Compiler compiler {"-1e1-", program};
        auto constant_number = ConstNumCompiler::create(compiler);
        auto data_type = constant_number->compile();
        REQUIRE_OPERAND(DataType::Double, "-1e1");
        REQUIRE(compiler.peekNextChar() == '-');
        REQUIRE(constant_number->unparsedChar() == 0);
    }
    SECTION("look for possible operator status (true if 'E' followed by another letter)")
    {
        Compiler compiler {"-1eqv", program};
        auto constant_number = ConstNumCompiler::create(compiler);
        auto data_type = constant_number->compile();
        REQUIRE_OPERAND(DataType::Integer, "-1");
        REQUIRE(compiler.peekNextChar() == 'q');
        REQUIRE(constant_number->unparsedChar() == 'e');
    }
    SECTION("check when 'E' character not followed by a another letter throws an error")
    {
        Compiler compiler {"-1e$", program};
        auto constant_number = ConstNumCompiler::create(compiler);

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(constant_number->compile(), CompileError);
        }
        SECTION("check the message, column and length of the error thrown")
        {
            try {
                constant_number->compile();
            }
            catch (const CompileError &error) {
                std::string expected_what = "expected sign or digit for exponent";
                REQUIRE(error.what() == expected_what);
                REQUIRE(error.column == 3);
                REQUIRE(error.length == 1);
            }
        }
    }
}

TEST_CASE("check other numeric constants from the IBCP tests", "[other]")
{
    ProgramUnit program;

    SECTION("various error tests")
    {
        struct ErrorTest {
            const char *input;
            const std::string expected_what;
            unsigned expected_column;
        };

        std::vector<ErrorTest> error_tests = {
            {".A", "expected digit after decimal point", 1},
            {".e", "expected digit after decimal point", 1},
            {"..01", "expected digit after decimal point", 1},
            {".", "expected digit after decimal point", 1},
            {"1e", "expected sign or digit for exponent", 2}
        };

        SECTION("check that errors are thrown")
        {
            for (auto &test : error_tests) {
                Compiler compiler {test.input, program};

                CAPTURE(test.input);
                REQUIRE_THROWS_AS(ConstNumCompiler::create(compiler)->compile(), CompileError);
            }
        }
        SECTION("check the message, column and length of the errors thrown")
        {
            for (auto &test : error_tests) {
                Compiler compiler {test.input, program};

                CAPTURE(test.input);
                try {
                    ConstNumCompiler::create(compiler)->compile();
                }
                catch (const CompileError &error) {
                    REQUIRE(error.what() == test.expected_what);
                    REQUIRE(error.column == test.expected_column);
                    REQUIRE(error.length == 1);
                }
            }
        }
    }
    SECTION("miscellaneous test")
    {
        Compiler compiler {"1..", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Double, "1.");
        REQUIRE(compiler.peekNextChar() == '.');
    }
}

TEST_CASE("recreate a constant", "[recreate]")
{
    ProgramUnit program;
    Compiler compiler {"", program};

    SECTION("recreate an integer constant")
    {
        compiler.addNumConstInstruction(false, "12345", 0);
        auto code_line = compiler.getCodeLine();
        program.appendCodeLine(code_line);

        REQUIRE(program.recreateLine(0) == "12345");
    }
}

TEST_CASE("execute a constant code", "[execute]")
{
    ProgramUnit program;
    std::ostringstream unused_oss;

    Compiler compiler {"", program};

    SECTION("execute a single integer constant")
    {
        compiler.addNumConstInstruction(false, "12345", 0);
        auto code_line = compiler.getCodeLine();
        program.appendCodeLine(code_line);

        auto executer = program.createExecutor(unused_oss);
        executer.executeOneCode();
        REQUIRE(executer.topInt() == 12345);
    }
    SECTION("execute a different integer constant")
    {
        compiler.addNumConstInstruction(false, "23456", 0);
        auto code_line = compiler.getCodeLine();
        program.appendCodeLine(code_line);

        auto executer = program.createExecutor(unused_oss);
        executer.executeOneCode();
        REQUIRE(executer.topInt() == 23456);
    }
    SECTION("execute two consecutive integer constants")
    {
        compiler.addNumConstInstruction(false, "12345", 0);
        compiler.addNumConstInstruction(false, "23456", 0);
        auto code_line = compiler.getCodeLine();
        program.appendCodeLine(code_line);

        auto executer = program.createExecutor(unused_oss);
        executer.executeOneCode();
        REQUIRE(executer.topInt() == 12345);
        executer.executeOneCode();
        REQUIRE(executer.topInt() == 23456);
    }
    SECTION("execute two consecutive integer constants and then check evaluation stack")
    {
        compiler.addNumConstInstruction(false, "12345", 0);
        compiler.addNumConstInstruction(false, "23456", 0);
        auto code_line = compiler.getCodeLine();
        program.appendCodeLine(code_line);

        auto executer = program.createExecutor(unused_oss);
        executer.executeOneCode();
        executer.executeOneCode();
        REQUIRE(executer.topInt() == 23456);
        executer.pop();
        REQUIRE(executer.topInt() == 12345);
    }
    SECTION("execute a single double constant")
    {
        compiler.addNumConstInstruction(true, "12.345", 0);
        auto code_line = compiler.getCodeLine();
        program.appendCodeLine(code_line);

        auto executer = program.createExecutor(unused_oss);
        executer.executeOneCode();
        REQUIRE(executer.topDbl() == 12.345);
    }
    SECTION("execute a different double constant")
    {
        compiler.addNumConstInstruction(true, "-2.3456", 0);
        auto code_line = compiler.getCodeLine();
        program.appendCodeLine(code_line);

        auto executer = program.createExecutor(unused_oss);
        executer.executeOneCode();
        REQUIRE(executer.topDbl() == -2.3456);
    }
    SECTION("check that same constant reuses existing entry in dictionary")
    {
        compiler.addNumConstInstruction(false, "12345", 0);
        compiler.addNumConstInstruction(false, "12345", 0);
        compiler.addNumConstInstruction(false, "23456", 0);

        auto code_line = compiler.getCodeLine();
        program.appendCodeLine(code_line);

        auto executer = program.createExecutor(unused_oss);
        executer.executeOneCode();
        REQUIRE(executer.topInt() == 12345);
        executer.executeOneCode();
        REQUIRE(executer.topInt() == 12345);
        executer.executeOneCode();
        REQUIRE(executer.topInt() == 23456);
    }
}

TEST_CASE("constant errors", "[errors]")
{
    ProgramUnit program;

    SECTION("compile an integer constant too large for an integer value (change to a double)")
    {
        extern Code const_dbl_code;

        Compiler compiler {"12345678901", program};
        auto data_type = ConstNumCompiler::create(compiler)->compile();
        REQUIRE_OPERAND(DataType::Double, "12345678901")
        REQUIRE(code_line[0].instructionCode()->getValue() == const_dbl_code.getValue());
    }
    SECTION("compile a double constant too large for a double value (an error)")
    {
        extern Code const_dbl_code;

        Compiler compiler {"1.23e4567", program};
        SECTION("check that error is thrown")
        {
            REQUIRE_THROWS_AS(ConstNumCompiler::create(compiler)->compile(), CompileError);
        }
        SECTION("check the message, column and length of the error thrown")
        {
            try {
                ConstNumCompiler::create(compiler)->compile();
            }
            catch (const CompileError &error) {
                std::string expected = "floating point constant is out of range";
                REQUIRE(error.what() == expected);
                REQUIRE(error.column == 0);
                REQUIRE(error.length == 9);
            }
        }
    }
}

TEST_CASE("changing a constant from double to integer", "[convert]")
{
    ProgramUnit program;

    SECTION("round positive double constant up (away from zero)")
    {
        std::istringstream iss {"PRINT 123.5 OR 0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "124\n");
    }
    SECTION("round negative double constant down (away from zero)")
    {
        std::istringstream iss {"PRINT -123.5 OR 0"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-124\n");
    }
    SECTION("check within upper limit of a double convertable to an integer")
    {
        std::istringstream iss {"PRINT 2147483647.499 OR 0"};
        std::ostringstream oss;

        program.compileSource(iss, oss);
        program.runCode(oss);

        REQUIRE(oss.str() == "2147483647\n");
    }
    SECTION("check outside upper limit of a double convertable to an integer")
    {
        std::istringstream iss {"PRINT 2147483647.5 OR 0"};
        std::ostringstream oss;

        program.compileSource(iss, oss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "error on line 1:7: floating point constant is out of range\n"
            "    PRINT 2147483647.5 OR 0\n"
            "          ^^^^^^^^^^^^\n"
        );
    }
    SECTION("check within lower limit of a double convertable to an integer")
    {
        std::istringstream iss {"PRINT -2147483648.499 OR 0"};
        std::ostringstream oss;

        program.compileSource(iss, oss);
        program.runCode(oss);

        REQUIRE(oss.str() == "-2147483648\n");
    }
    SECTION("check outside lower limit of a double convertable to an integer")
    {
        std::istringstream iss {"PRINT -2147483648.5 OR 0"};
        std::ostringstream oss;

        program.compileSource(iss, oss);
        program.runCode(oss);

        REQUIRE(oss.str() ==
            "error on line 1:7: floating point constant is out of range\n"
            "    PRINT -2147483648.5 OR 0\n"
            "          ^^^^^^^^^^^^^\n"
        );
    }
}
