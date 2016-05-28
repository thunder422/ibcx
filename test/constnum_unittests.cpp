/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "compiler.h"
#include "constnum.h"
#include "constnumcompiler.h"
#include "compileerror.h"
#include "programcode.h"
#include "programunit.h"
#include "recreator.h"
#include "support.h"


TEST_CASE("compiling integer constants from a string", "[integers]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("input stream does not contain a constant (caller will determine action)")
    {
        std::istringstream iss {"%"};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE(data_type == DataType::Null);
        REQUIRE(code_line.size() == 0);
        REQUIRE(iss.peek() == '%');
    }
    SECTION("compile a single digit number")
    {
        extern Code const_int_code;

        std::istringstream iss {"1"};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE_OPERAND(DataType::Integer, "1");
        REQUIRE(code_line[0].instructionCode()->getValue() == const_int_code.getValue());
    }
    SECTION("compile a multiple digit number")
    {
        std::istringstream iss {"123"};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE_OPERAND(DataType::Integer, "123");
    }
    SECTION("compile a negative number")
    {
        std::istringstream iss {"-234"};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE_OPERAND(DataType::Integer, "-234");
    }
    SECTION("terminate compiling at correct character")
    {
        std::istringstream iss {"345+"};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE_OPERAND(DataType::Integer, "345");
        REQUIRE(iss.peek() == '+');
    }
}


TEST_CASE("compiling floating point constants from a string", "[doubles]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("compile a number with a decimal point")
    {
        std::istringstream iss {"0.5"};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE_OPERAND(DataType::Double, "0.5");
    }
    SECTION("compile a number with a decimal point at the beginning")
    {
        std::istringstream iss {".75"};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE_OPERAND(DataType::Double, ".75");
    }
    SECTION("compile a number with a second decimal point (should ignore second one)")
    {
        std::istringstream iss {"0.1."};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE_OPERAND(DataType::Double, "0.1");
        REQUIRE(iss.peek() == '.');
    }
    SECTION("compile a number to a double constant in the code_line")
    {
        extern Code const_dbl_code;

        std::istringstream iss {"1.2"};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE_OPERAND(DataType::Double, "1.2");
        REQUIRE(code_line[0].instructionCode()->getValue() == const_dbl_code.getValue());
    }
    SECTION("compile a number with an exponent")
    {
        std::istringstream iss {"1e0"};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE_OPERAND(DataType::Double, "1e0");
    }
    SECTION("make sure compiling stops before a second 'E'")
    {
        std::istringstream iss {"1e0E"};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE_OPERAND(DataType::Double, "1e0");
        REQUIRE(iss.peek() == 'E');
    }
    SECTION("compile a number with a minus exponent")
    {
        std::istringstream iss {"1e-2"};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE_OPERAND(DataType::Double, "1e-2");
    }
    SECTION("compile a number with a minus exponent terminated be a minus operator")
    {
        std::istringstream iss {"1e-2-"};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE_OPERAND(DataType::Double, "1e-2");
        REQUIRE(iss.peek() == '-');
    }
    SECTION("compile a number with a plus exponent")
    {
        std::istringstream iss {"1e+2"};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE_OPERAND(DataType::Double, "1e+2");
    }
}


TEST_CASE("handle leading zero of a constant correctly including errors", "[zero]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("check for an error when a leading zero is not followed by a digit")
    {
        std::istringstream iss {"01"};
        Compiler compiler {iss, code_line, program};
        ConstNumCompiler compile_constant {compiler};
        REQUIRE_THROWS_AS(compile_constant(), CompileError);
    }
    SECTION("check for the correct error message and a column")
    {
        std::istringstream iss {"01"};
        Compiler compiler {iss, code_line, program};
        try {
            ConstNumCompiler{compiler}();
        }
        catch (const CompileError &error) {
            std::string expected = "expected decimal point after leading zero";
            REQUIRE(error.what() == expected);
            REQUIRE(error.column == 1);
        }
    }
    SECTION("check for the correct error column")
    {
        std::istringstream iss {"word 01"};
        Compiler compiler {iss, code_line, program};
        std::string skip_word;
        iss >> skip_word >> std::ws;
        try {
            ConstNumCompiler{compiler}();
        }
        catch (const CompileError &error) {
            std::string expected = "expected decimal point after leading zero";
            REQUIRE(error.what() == expected);
            REQUIRE(error.column == 6);
        }
    }
    SECTION("check compiling ends when followed by a non-period non-digit")
    {
        std::istringstream iss {"0-"};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE(data_type == DataType::Integer);
        REQUIRE_OPERAND(DataType::Integer, "0");
        REQUIRE(iss.peek() == '-');
    }
}


TEST_CASE("handle leading period of a constant correctly including errors", "[period]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("check for an error when a leading period is followed by another period")
    {
        std::istringstream iss {".."};
        Compiler compiler {iss, code_line, program};
        ConstNumCompiler compile_constant {compiler};
        REQUIRE_THROWS_AS(compile_constant(), CompileError);
    }
    SECTION("check error message and column if no digits after period")
    {
        std::istringstream iss {".."};
        Compiler compiler {iss, code_line, program};
        try {
            ConstNumCompiler{compiler}();
        }
        catch (const CompileError &error) {
            std::string expected = "expected digit after decimal point";
            REQUIRE(error.what() == expected);
            REQUIRE(error.column == 1);
        }
    }
}


TEST_CASE("check for correct exponent format", "[exponent]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("check for sign or digits at the start of an exponent")
    {
        std::istringstream iss {"1e."};
        Compiler compiler {iss, code_line, program};
        ConstNumCompiler compile_constant {compiler};
        REQUIRE_THROWS_AS(compile_constant(), CompileError);
    }
    SECTION("check error message and column if no sign or digits at start of exponent")
    {
        std::istringstream iss {"1e."};
        Compiler compiler {iss, code_line, program};
        try {
            ConstNumCompiler{compiler}();
        }
        catch (const CompileError &error) {
            std::string expected = "expected sign or digit for exponent";
            REQUIRE(error.what() == expected);
            REQUIRE(error.column == 2);
        }
    }
    SECTION("allow for possible EQV operator ('E' lost, which will be handled by caller)")
    {
        std::istringstream iss {"1eq"};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE_OPERAND(DataType::Integer, "1");
        REQUIRE(iss.peek() == 'q');
    }
    SECTION("make sure there is a digit after an exponent sign")
    {
        std::istringstream iss {"1e+"};
        Compiler compiler {iss, code_line, program};
        ConstNumCompiler compile_constant {compiler};
        REQUIRE_THROWS_AS(compile_constant(), CompileError);
    }
    SECTION("check error message and column if no digit after exponent sign")
    {
        std::istringstream iss {"1e-A"};
        Compiler compiler {iss, code_line, program};
        try {
            ConstNumCompiler{compiler}();
        }
        catch (const CompileError &error) {
            std::string expected = "expected digit after exponent sign";
            REQUIRE(error.what() == expected);
            REQUIRE(error.column == 3);
        }
    }
    SECTION("check correct error column when terminated by end of line")
    {
        std::istringstream iss {"1e-"};
        Compiler compiler {iss, code_line, program};
        try {
            ConstNumCompiler{compiler}();
        }
        catch (const CompileError &error) {
            REQUIRE(error.column == 3);
        }

    }
}


TEST_CASE("look for possible exit conditions", "[exit]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("look for possible negate operator ('-' not followed by '.' or digit)")
    {
        std::istringstream iss {"-e"};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE(data_type == DataType::Null);
        REQUIRE(code_line.size() == 0);
    }
    SECTION("allow a period after a negative sign")
    {
        std::istringstream iss {"-.1"};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE_OPERAND(DataType::Double, "-.1");
    }
    SECTION("look for negate operator status (false if not negate operator)")
    {
        std::istringstream iss {"-1-"};
        Compiler compiler {iss, code_line, program};
        ConstNumCompiler compile_constant {compiler};
        auto data_type = compile_constant();
        REQUIRE_OPERAND(DataType::Integer, "-1");
        REQUIRE(iss.peek() == '-');
        REQUIRE_FALSE(compile_constant.negateOperator());
    }
    SECTION("look for negate operator status (true for negate operator)")
    {
        std::istringstream iss {"-e"};
        Compiler compiler {iss, code_line, program};
        ConstNumCompiler compile_constant {compiler};
        auto data_type = compile_constant();
        REQUIRE(data_type == DataType::Null);
        REQUIRE(iss.peek() == 'e');
        REQUIRE(compile_constant.negateOperator());
    }
    SECTION("look for possible operator status (false if no operator starting with 'E')")
    {
        std::istringstream iss {"-1e1-"};
        Compiler compiler {iss, code_line, program};
        ConstNumCompiler compile_constant {compiler};
        auto data_type = compile_constant();
        REQUIRE_OPERAND(DataType::Double, "-1e1");
        REQUIRE(iss.peek() == '-');
        REQUIRE_FALSE(compile_constant.possibleOperator());
    }
    SECTION("look for possible operator status (true if 'E' followed by another letter)")
    {
        std::istringstream iss {"-1eqv"};
        Compiler compiler {iss, code_line, program};
        ConstNumCompiler compile_constant {compiler};
        auto data_type = compile_constant();
        REQUIRE_OPERAND(DataType::Integer, "-1");
        REQUIRE(iss.peek() == 'q');
        REQUIRE(compile_constant.possibleOperator());
    }
    SECTION("check when 'E' character not followed by a another letter throws an error")
    {
        std::istringstream iss {"-1e$"};
        Compiler compiler {iss, code_line, program};
        ConstNumCompiler compile_constant {compiler};
        REQUIRE_THROWS_AS(compile_constant(), CompileError);
    }
    SECTION("check error message and column when 'E' character not followed by a another letter")
    {
        std::istringstream iss {"-1e$"};
        Compiler compiler {iss, code_line, program};
        try {
            ConstNumCompiler{compiler}();
        }
        catch (const CompileError &error) {
            std::string expected_what = "expected sign or digit for exponent";
            REQUIRE(error.what() == expected_what);
            REQUIRE(error.column == 3);
        }
    }
}


bool test_error_input(const char *input, const std::string &expected_what, unsigned expected_column)
{
    ProgramUnit program;
    ProgramCode code_line;

    CAPTURE(input);
    std::istringstream iss {input};
    Compiler compiler {iss, code_line, program};
    try {
        ConstNumCompiler{compiler}();
        return false;
    }
    catch (const CompileError &error) {
        REQUIRE(error.what() == expected_what);
        REQUIRE(error.column == expected_column);
        return true;
    }
}

TEST_CASE("check other numeric constants from the IBCP tests", "[other]")
{
    ProgramUnit program;
    ProgramCode code_line;

    SECTION("various error tests")
    {
        struct ErrorTest {
            const char *input;
            const char *expected_what;
            unsigned expected_column;
        };

        std::vector<ErrorTest> error_tests = {
            {".A", "expected digit after decimal point", 1},
            {".e", "expected digit after decimal point", 1},
            {"..01", "expected digit after decimal point", 1},
            {".", "expected digit after decimal point", 1},
            {"1e", "expected sign or digit for exponent", 2}
        };

        for (auto &test : error_tests) {
            CAPTURE(test.input);
            REQUIRE(test_error_input(test.input, test.expected_what, test.expected_column));
        }
    }
    SECTION("miscellenous test")
    {
        std::istringstream iss {"1.."};
        Compiler compiler {iss, code_line, program};
        auto data_type = ConstNumCompiler{compiler}();
        REQUIRE_OPERAND(DataType::Double, "1.");
        REQUIRE(iss.peek() == '.');
    }
}

TEST_CASE("recreate a constant", "[recreate]")
{
    ProgramUnit program;
    ProgramCode code;
    std::istringstream not_used_iss;
    Compiler compiler {not_used_iss, code, program};

    SECTION("recreate an integer constant")
    {
        extern ConstNumCode const_int_code;
        compiler.addConstNumInstruction(const_int_code, "12345");
        program.addCodeLine(code);

        Recreator recreator {program};
        recreator.recreateOneCode();
        REQUIRE(recreator.top() == "12345");
    }
}
