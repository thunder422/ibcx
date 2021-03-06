/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "catch.hpp"
#include "compiler.h"
#include "compileerror.h"
#include "executer.h"
#include "operators.h"
#include "programerror.h"
#include "programunit.h"


TEST_CASE("string constants", "[const][compile]")
{
    ProgramUnit program;

    SECTION("parse the initial double quote character")
    {
        Compiler compiler {R"("test")", program};

        compiler.compileStringConstant();
        REQUIRE(compiler.peekNextChar() != '"');
    }
    SECTION("parse past the ending double quote character")
    {
        Compiler compiler {R"("test")", program};

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
        Compiler compiler {R"("test"+)", program};

        compiler.compileStringConstant();
        REQUIRE(compiler.peekNextChar() == '+');
    }
    SECTION("parse two sequential double quotes as part of the constant")
    {
        Compiler compiler {R"("te""st")", program};

        compiler.compileStringConstant();
        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check for a null return data type when there is no string constant")
    {
        Compiler compiler {"123", program};

        auto data_type = compiler.compileStringConstant();
        REQUIRE_FALSE(data_type);
    }
    SECTION("check for a string return data type when there is a string constant")
    {
        Compiler compiler {R"("test")", program};

        auto data_type = compiler.compileStringConstant();
        REQUIRE(data_type.isString());
    }
    SECTION("check that a string constant is added to the program code")
    {
        Compiler compiler {R"("test")", program};

        compiler.compileStringConstant();
        auto code_line = compiler.getCodeLine();

        extern Code const_str_code;
        REQUIRE(code_line.size() == 2);
        REQUIRE(code_line[0].instructionCode()->getValue() == const_str_code.getValue());
    }
    SECTION("check that each string has a unique operand (index) value")
    {
        Compiler compiler1 {R"("first")", program};
        compiler1.compileStringConstant();
        Compiler compiler2 {R"("second")", program};
        compiler2.compileStringConstant();

        auto code_line1 = compiler1.getCodeLine();
        auto code_line2 = compiler2.getCodeLine();

        REQUIRE(code_line1.size() == 2);
        auto operand = code_line1[1].operand();
        REQUIRE(operand == 0);
        REQUIRE(program.getConstantString(operand) == "first");

        REQUIRE(code_line2.size() == 2);
        operand = code_line2[1].operand();
        REQUIRE(operand == 1);
        REQUIRE(program.getConstantString(operand) == "second");
    }
    SECTION("check that an empty string is compiled correctly")
    {
        Compiler compiler {R"("")", program};
        compiler.compileStringConstant();

        auto code_line = compiler.getCodeLine();

        REQUIRE(code_line.size() == 2);
        auto operand = code_line[1].operand();
        REQUIRE(program.getConstantString(operand) == "");
    }
    SECTION("check that a string with an embedded double quote is stored correctly")
    {
        Compiler compiler {R"("te""st")", program};

        compiler.compileStringConstant();
        auto code_line = compiler.getCodeLine();

        REQUIRE(code_line.size() == 2);
        auto operand = code_line[1].operand();
        REQUIRE(program.getConstantString(operand) == R"(te"st)");
    }

    SECTION("check that a string constant is compiled as part of an expression")
    {
        Compiler compiler {R"("test")", program};

        auto data_type = compiler.compileExpression();

        REQUIRE(data_type.isString());
    }
    SECTION("check for an error if used as the left side operand of a math operator")
    {
        Compiler compiler {R"(2+"test"^5)", program};

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(compiler.compileExpression(), ExpNumExprError);
        }
        SECTION("check the column and length of the error thrown")
        {
            try {
                compiler.compileExpression();
            }
            catch (const ExpNumExprError &error) {
                REQUIRE(error.column == 2);
                REQUIRE(error.length == 6);
            }
        }
    }
    SECTION("check for an error if used as the right side operand of a math operator")
    {
        Compiler compiler {R"(23+"te""st")", program};

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(compiler.compileExpression(), ExpNumExprError);
        }
        SECTION("check the column and length of the error thrown")
        {
            try {
                compiler.compileExpression();
            }
            catch (const ExpNumExprError &error) {
                REQUIRE(error.column == 3);
                REQUIRE(error.length == 8);
            }
        }
    }
    SECTION("check for an error if used as the operand of a negate operator")
    {
        Compiler compiler {R"(-"bad")", program};

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(compiler.compileExpression(), ExpNumExprError);
        }
        SECTION("check the column and length of the error thrown")
        {
            try {
                compiler.compileExpression();
            }
            catch (const ExpNumExprError &error) {
                REQUIRE(error.column == 1);
                REQUIRE(error.length == 5);
            }
        }
    }
    SECTION("check for an error if used as the operand of a not operator")
    {
        Compiler compiler {R"(NOT"bad")", program};

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(compiler.compileExpression(), ExpNumExprError);
        }
        SECTION("check the column and length of the error thrown")
        {
            try {
                compiler.compileExpression();
            }
            catch (const ExpNumExprError &error) {
                REQUIRE(error.column == 3);
                REQUIRE(error.length == 5);
            }
        }
    }
    SECTION("check for an error if used as an argument to a multi-type function")
    {
        Compiler compiler {R"*(ABS("bad"))*", program};

        SECTION("check that the error is thrown")
        {
            REQUIRE_THROWS_AS(compiler.compileExpression(), ExpNumExprError);
        }
        SECTION("check the column and length of the error thrown")
        {
            try {
                compiler.compileExpression();
            }
            catch (const ExpNumExprError &error) {
                REQUIRE(error.column == 4);
                REQUIRE(error.length == 5);
            }
        }
    }
}

TEST_CASE("recreate string constants", "[const][recreate]")
{
    ProgramUnit program;

    SECTION("recreate an string constant")
    {
        Compiler compiler {R"("test")", program};
        compiler.compileStringConstant();
        auto code_line = compiler.getCodeLine();
        program.appendCodeLine(code_line);

        REQUIRE(program.recreateLine(0) == R"("test")");
    }
    SECTION("recreate an string constant with an embedded double quote")
    {
        Compiler compiler {R"("te""st")", program};
        compiler.compileStringConstant();
        auto code_line = compiler.getCodeLine();
        program.appendCodeLine(code_line);

        REQUIRE(program.recreateLine(0) == R"("te""st")");
    }
}

TEST_CASE("execute string constants", "[const][execute]")
{
    ProgramUnit program;
    std::ostringstream unused_oss;

    Compiler compiler {"", program};

    SECTION("execute a single string constant")
    {
        compiler.addStrConstInstruction("test123");
        auto code_line = compiler.getCodeLine();
        program.appendCodeLine(code_line);

        auto executer = program.createExecuter(unused_oss);
        executer.executeOneCode();
        REQUIRE(*executer.topStr() == "test123");
    }
}


TEST_CASE("relational operators with string operands", "[relational]")
{
    ProgramUnit program;

    SECTION("make sure both string operands and relation operator are parsed")
    {
        Compiler compiler {R"("left"<"test")", program};
        compiler.compileExpression();

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check for an relation operator code after the operands")
    {
        Compiler compiler {R"("left"<"test")", program};
        compiler.compileExpression();
        auto code_line = compiler.getCodeLine();

        extern OperatorCode<OpType::StrStr> lt_str_str_code;
        REQUIRE(code_line.size() == 5);
        REQUIRE(code_line[4].instructionCode()->getValue() == lt_str_str_code.getValue());
    }
    SECTION("check for an error when string on right side with number on left side")
    {
        std::istringstream iss {R"(PRINT 5<"right")"};
        std::ostringstream oss;

        program.compileSource(iss, oss);

        REQUIRE(oss.str() ==
            R"(error on line 1:9: expected numeric expression)" "\n"
            R"(    PRINT 5<"right")" "\n"
            R"(            ^^^^^^^)" "\n"
        );
    }
    SECTION("check for an error when number on right side with string on left side")
    {
        std::istringstream iss {R"(PRINT "left"<5)"};
        std::ostringstream oss;

        program.compileSource(iss, oss);

        REQUIRE(oss.str() ==
            R"(error on line 1:14: expected string expression)" "\n"
            R"(    PRINT "left"<5)" "\n"
            R"(                 ^)" "\n"
        );
    }

    SECTION("recreate less-than operator with string operands")
    {
        std::istringstream iss {R"(PRINT "left"<"right")"};
        std::ostringstream oss;

        program.compileSource(iss, oss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left" < "right")" "\n");
    }
    SECTION("recreate greater-than operator with string operands")
    {
        std::istringstream iss {R"(PRINT "left">"right")"};
        std::ostringstream oss;

        program.compileSource(iss, oss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left" > "right")" "\n");
    }
    SECTION("recreate less-than or equal operator with string operands")
    {
        std::istringstream iss {R"(PRINT "left"<="right")"};
        std::ostringstream oss;

        program.compileSource(iss, oss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left" <= "right")" "\n");
    }
    SECTION("recreate greater-than or equal operator with string operands")
    {
        std::istringstream iss {R"(PRINT "left">="right")"};
        std::ostringstream oss;

        program.compileSource(iss, oss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left" >= "right")" "\n");
    }

    SECTION("execute less-than operator with string operands")
    {
        std::istringstream iss {
            R"(PRINT "left"<"right")" "\n"
            R"(PRINT "left"<"RIGHT")" "\n"
            R"(PRINT "same"<"same")" "\n"
        };
        std::ostringstream oss;

        program.compileSource(iss, oss);
        program.run(oss);

        REQUIRE(oss.str() == "-1\n0\n0\n");
    }
    SECTION("execute greater-than operator with string operands")
    {
        std::istringstream iss {
            R"(PRINT "left">"right")" "\n"
            R"(PRINT "left" >"RIGHT")" "\n"
            R"(PRINT "same"> "same")" "\n"
        };
        std::ostringstream oss;

        program.compileSource(iss, oss);
        program.run(oss);

        REQUIRE(oss.str() == "0\n-1\n0\n");
    }
    SECTION("execute less-than or-equal operator with string operands")
    {
        std::istringstream iss {
            R"(PRINT "left"<="right")" "\n"
            R"(PRINT "left"<="RIGHT")" "\n"
            R"(PRINT "same"<="same")" "\n"
        };
        std::ostringstream oss;

        program.compileSource(iss, oss);
        program.run(oss);

        REQUIRE(oss.str() == "-1\n0\n-1\n");
    }
    SECTION("execute greater-than or-equal operator with string operands")
    {
        std::istringstream iss {
            R"(PRINT "left">="right")" "\n"
            R"(PRINT "left">="RIGHT")" "\n"
            R"(PRINT "same">="same")" "\n"
        };
        std::ostringstream oss;

        program.compileSource(iss, oss);
        program.run(oss);

        REQUIRE(oss.str() == "0\n-1\n-1\n");
    }
}


TEST_CASE("equality operators with string operands", "[equality]")
{
    ProgramUnit program;

    SECTION("make sure both string operands and equality operator are parsed")
    {
        Compiler compiler {R"("left"="right")", program};
        compiler.compileExpression();

        REQUIRE(compiler.peekNextChar() == EOF);
    }

    SECTION("recreate equal operator with string operands")
    {
        std::istringstream iss {R"(PRINT "left"="right")"};
        std::ostringstream oss;

        program.compileSource(iss, oss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left" = "right")" "\n");
    }
    SECTION("recreate not-equal operator with string operands")
    {
        std::istringstream iss {R"(PRINT "left"<>"right")"};
        std::ostringstream oss;

        program.compileSource(iss, oss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left" <> "right")" "\n");
    }

    SECTION("execute equal operator with string operands")
    {
        std::istringstream iss {
            R"(PRINT "left"="right")" "\n"
            R"(PRINT "left"="RIGHT")" "\n"
            R"(PRINT "same"="same")" "\n"
        };
        std::ostringstream oss;

        program.compileSource(iss, oss);
        program.run(oss);

        REQUIRE(oss.str() == "0\n0\n-1\n");
    }
    SECTION("execute not-equal operator with string operands")
    {
        std::istringstream iss {
            R"(PRINT "left"<>"right")" "\n"
            R"(PRINT "left"<>"RIGHT")" "\n"
            R"(PRINT "same"<>"same")" "\n"
        };
        std::ostringstream oss;

        program.compileSource(iss, oss);
        program.run(oss);

        REQUIRE(oss.str() == "-1\n-1\n0\n");
    }
}


TEST_CASE("compile concatenation operator expressions", "[cat][compile]")
{
    ProgramUnit program;

    SECTION("make sure both string operands and operator are parsed")
    {
        Compiler compiler {R"("left"+"right")", program};
        compiler.compileExpression();

        REQUIRE(compiler.peekNextChar() == EOF);
    }
    SECTION("check for error if subtract operator has string operands")
    {
        std::istringstream iss {R"(PRINT "left"-"right")"};
        std::ostringstream oss;

        program.compileSource(iss, oss);

        REQUIRE(oss.str() ==
            R"(error on line 1:7: expected numeric expression)" "\n"
            R"(    PRINT "left"-"right")" "\n"
            R"(          ^^^^^^)" "\n"
        );
    }
    SECTION("check that concatenation expression has temporary string data type")
    {
        Compiler compiler {R"("left"+"right")", program};
        auto data_type = compiler.compileExpression();

        REQUIRE(data_type.isTmpStr());
    }
    SECTION("check that parentheses allow string expressions")
    {
        Compiler compiler {R"(("left"+"right"))", program};
        auto data_type = compiler.compileExpression();

        REQUIRE(data_type.isTmpStr());
    }
    SECTION("compile with right side temporary string operand")
    {
        Compiler compiler {R"("left"+("right1"+"right2"))", program};

        compiler.compileExpression();
        auto code_line = compiler.getCodeLine();

        extern Code cat_str_tmp_code;
        REQUIRE(code_line.size() == 8);
        REQUIRE(code_line[7].instructionCode()->getValue() == cat_str_tmp_code.getValue());
    }
    SECTION("compile with left side temporary string operand")
    {
        Compiler compiler {R"("left1"+"left2"+"right")", program};

        compiler.compileExpression();
        auto code_line = compiler.getCodeLine();

        extern Code cat_tmp_str_code;
        REQUIRE(code_line.size() == 8);
        REQUIRE(code_line[7].instructionCode()->getValue() == cat_tmp_str_code.getValue());
    }
    SECTION("compile with left and right side temporary string operands")
    {
        Compiler compiler {R"("left1"+"left2"+("right1"+"right2"))", program};

        compiler.compileExpression();
        auto code_line = compiler.getCodeLine();

        extern Code cat_tmp_tmp_code;
        REQUIRE(code_line.size() == 11);
        REQUIRE(code_line[10].instructionCode()->getValue() == cat_tmp_tmp_code.getValue());
    }
    SECTION("check for error with the left side temporary string on and right side not a string")
    {
        std::istringstream iss {R"(PRINT "left1"+"left2"+123)"};
        std::ostringstream oss;

        program.compileSource(iss, oss);

        REQUIRE(oss.str() ==
            R"(error on line 1:23: expected string expression)" "\n"
            R"(    PRINT "left1"+"left2"+123)" "\n"
            R"(                          ^^^)" "\n"
        );
    }
}

TEST_CASE("recreate expressions with concatentation and temporary strings", "[cat][recreate]")
{
    ProgramUnit program;

    SECTION("concatenation with two string operands")
    {
        std::istringstream iss {R"(PRINT "left"+"right")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left" + "right")" "\n");
    }
    SECTION("concatenation with left temporary string and right string operands")
    {
        std::istringstream iss {R"(PRINT "left1"+"left2"+"right")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left1" + "left2" + "right")" "\n");
    }
    SECTION("concatenation with left string and right temporary string operands")
    {
        std::istringstream iss {R"(PRINT "left"+("right1"+"right2"))"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left" + ("right1" + "right2"))" "\n");
    }
    SECTION("concatenation with two temporary string operands")
    {
        std::istringstream iss {R"(PRINT "left1"+"left2"+("right1"+"right2"))"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left1" + "left2" + ("right1" + "right2"))" "\n");
    }
}

TEST_CASE("recreate relational expressions with temporary strings", "[relational][recreate]")
{
    ProgramUnit program;

    SECTION("less-than with left temporary string and right string operands")
    {
        std::istringstream iss {R"(PRINT "left1"+"left2"<"right")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left1" + "left2" < "right")" "\n");
    }
    SECTION("less-than with left string and right temporary string operands")
    {
        std::istringstream iss {R"(PRINT "left"<"right1"+"right2")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left" < "right1" + "right2")" "\n");
    }
    SECTION("less-than with two temporary string operands")
    {
        std::istringstream iss {R"(PRINT "left1"+"left2"<"right1"+"right2")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left1" + "left2" < "right1" + "right2")" "\n");
    }

    SECTION("greater-than with left temporary string and right string operands")
    {
        std::istringstream iss {R"(PRINT "left1"+"left2">"right")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left1" + "left2" > "right")" "\n");
    }
    SECTION("greater-than with left string and right temporary string operands")
    {
        std::istringstream iss {R"(PRINT "left">"right1"+"right2")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left" > "right1" + "right2")" "\n");
    }
    SECTION("greater-than with two temporary string operands")
    {
        std::istringstream iss {R"(PRINT "left1"+"left2">"right1"+"right2")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left1" + "left2" > "right1" + "right2")" "\n");
    }

    SECTION("less-than-or-equal with left temporary string and right string operands")
    {
        std::istringstream iss {R"(PRINT "left1"+"left2"<="right")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left1" + "left2" <= "right")" "\n");
    }
    SECTION("less-than-or-equal with left string and right temporary string operands")
    {
        std::istringstream iss {R"(PRINT "left"<="right1"+"right2")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left" <= "right1" + "right2")" "\n");
    }
    SECTION("less-than-or-equal with two temporary string operands")
    {
        std::istringstream iss {R"(PRINT "left1"+"left2"<="right1"+"right2")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left1" + "left2" <= "right1" + "right2")" "\n");
    }

    SECTION("greater-than-or-equal with left temporary string and right string operands")
    {
        std::istringstream iss {R"(PRINT "left1"+"left2">="right")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left1" + "left2" >= "right")" "\n");
    }
    SECTION("greater-than-or-equal with left string and right temporary string operands")
    {
        std::istringstream iss {R"(PRINT "left">="right1"+"right2")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left" >= "right1" + "right2")" "\n");
    }
    SECTION("greater-than-or-equal with two temporary string operands")
    {
        std::istringstream iss {R"(PRINT "left1"+"left2">="right1"+"right2")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left1" + "left2" >= "right1" + "right2")" "\n");
    }
}

TEST_CASE("recreate equality expressions with temporary strings", "[equality][recreate]")
{
    ProgramUnit program;

    SECTION("equal with left temporary string and right string operands")
    {
        std::istringstream iss {R"(PRINT "left1"+"left2"="right")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left1" + "left2" = "right")" "\n");
    }
    SECTION("equal with left string and right temporary string operands")
    {
        std::istringstream iss {R"(PRINT "left"="right1"+"right2")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left" = "right1" + "right2")" "\n");
    }
    SECTION("equal with two temporary string operands")
    {
        std::istringstream iss {R"(PRINT "left1"+"left2"="right1"+"right2")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left1" + "left2" = "right1" + "right2")" "\n");
    }

    SECTION("not-equal with left temporary string and right string operands")
    {
        std::istringstream iss {R"(PRINT "left1"+"left2"<>"right")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left1" + "left2" <> "right")" "\n");
    }
    SECTION("not-equal with left string and right temporary string operands")
    {
        std::istringstream iss {R"(PRINT "left"<>"right1"+"right2")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left" <> "right1" + "right2")" "\n");
    }
    SECTION("not-equal with two temporary string operands")
    {
        std::istringstream iss {R"(PRINT "left1"+"left2"<>"right1"+"right2")"};
        std::ostringstream oss;

        program.compile(iss);
        program.recreate(oss);

        REQUIRE(oss.str() == R"(PRINT "left1" + "left2" <> "right1" + "right2")" "\n");
    }
}

TEST_CASE("execute concatentation expressions with temporary strings", "[cat][execute]")
{
    ProgramUnit program;

    SECTION("concatenation with two string operands")
    {
        std::istringstream iss {R"(PRINT "Left"+"Right")"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "LeftRight\n");
    }
    SECTION("concatenation with left temporary string and right string operands")
    {
        std::istringstream iss {R"(PRINT "Left1"+"Left2"+"Right")"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "Left1Left2Right\n");
    }
    SECTION("concatenation with left string and right temporary string operands")
    {
        std::istringstream iss {R"(PRINT "Left"+("Right1"+"Right2"))"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "LeftRight1Right2\n");
    }
    SECTION("concatenation with two temporary string operands")
    {
        std::istringstream iss {R"(PRINT "Left1"+"Left2"+("Right1"+"Right2"))"};
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "Left1Left2Right1Right2\n");
    }
}

TEST_CASE("execute relational expressions with temporary strings", "[relational][execute]")
{
    ProgramUnit program;

    SECTION("less-than with left temporary string and right string operands")
    {
        std::istringstream iss {
            R"(PRINT "a"+"a"<"bb")" "\n"
            R"(PRINT "c"+"c"<"bb")" "\n"
            R"(PRINT "b"+"b"<"bb")"
        };
        std::ostringstream oss;

        program.compileSource(iss, oss);
        program.run(oss);

        REQUIRE(oss.str() == "-1\n0\n0\n");
    }
    SECTION("less-than with left string and right temporary string operands")
    {
        std::istringstream iss {
            R"(PRINT "aa"<"b"+"b")" "\n"
            R"(PRINT "cc"<"b"+"b")" "\n"
            R"(PRINT "bb"<"b"+"b")"
        };
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-1\n0\n0\n");
    }
    SECTION("less-than with two temporary string operands")
    {
        std::istringstream iss {
            R"(PRINT "a"+"a"<"b"+"b")" "\n"
            R"(PRINT "c"+"c"<"b"+"b")" "\n"
            R"(PRINT "b"+"b"<"b"+"b")"
        };
        std::ostringstream oss;

        program.compile(iss);
        program.run(oss);

        REQUIRE(oss.str() == "-1\n0\n0\n");
    }
}
