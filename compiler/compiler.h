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
#include "programcode.h"


class Code;
class ProgramCode;
class ProgramUnit;

class Compiler {
public:
    Compiler(const std::string &line, ProgramUnit &program);

    ci_string getKeyword();
    int getKeywordColumn() const noexcept;
    char peekNextChar();
    char getNextChar();
    int getColumn();

    void addInstruction(Code &code);
    DataType addConstNumInstruction(bool floating_point, const std::string &number);
    ProgramCode &&getCodeLine();

private:
    std::istringstream iss;
    ProgramUnit &program;
    ProgramCode code_line;
    int keyword_column;
};


#endif  // IBC_COMPILER_H