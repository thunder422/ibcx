/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_COMPILER_H
#define IBC_COMPILER_H

#include <iosfwd>
#include <string>
#include <sstream>

#include "cistring.h"
#include "datatype.h"
#include "precedence.h"
#include "programcode.h"


class Code;
class ProgramCode;
class ProgramUnit;
class OperatorCodes;

class Compiler {
public:
    Compiler(const std::string &line, ProgramUnit &program);
    DataType compileExpression(DataType expected_data_type);
    OperatorCodes *getSymbolOperatorCodes(Precedence::Level precedence);
    OperatorCodes *getWordOperatorCodes(Precedence::Level precedence);
    OperatorCodes *getComparisonOperatorCodes(Precedence::Level precedence);

    ci_string getKeyword();
    void parseKeyword(char first_char);
    void clearWord();
    char peekNextChar();
    char getNextChar();
    void skipWhiteSpace();
    unsigned getColumn() noexcept;

    void addInstruction(Code &code);
    DataType addNumConstInstruction(bool floating_point, const std::string &number,
        unsigned column);
    void convertToDouble(DataType operand_data_type);
    void convertToInteger(DataType operand_data_type);
    ProgramCode &&getCodeLine();

private:
    friend class ComparisonOperatorCodes;

    OperatorCodes *savedEqualityOperatorCodes();
    void setEqualityCodes(OperatorCodes *codes);
    ci_string getAlphaOnlyWord();
    void changeConstantToDouble();
    void changeConstantToInteger();
    void validateConstantConvertibleToInteger(size_t last_constant_offset);

    std::istringstream iss;
    ProgramUnit &program;
    ProgramCode code_line;
    char peek_char {0};
    unsigned column {0};
    bool last_operand_was_constant;
    unsigned last_constant_column;
    unsigned last_constant_length;
    ci_string word;
    OperatorCodes *equality_codes;
    unsigned word_column {0};
};


#endif  // IBC_COMPILER_H
