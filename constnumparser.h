/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef CONSTANTNUMBERPARSER_H
#define CONSTANTNUMBERPARSER_H

#include <iosfwd>

#include "datatype.h"


class ProgramCode;
class ProgramUnit;
class State;

class ConstNumParser {
public:
    ConstNumParser(std::istream &is);

    DataType parse(ProgramCode &code_line, ProgramUnit &program);

    void changeState(State &new_state);
    unsigned getColumn() const;
    void addNextChar();
    void setDouble();
    bool isDouble();
    void setDone();
    void setPossibleOperator();

private:
    void processInput();

    std::istream &is;
    State *state;
    std::string number;
    bool floating_point;
    bool done;
    unsigned column;
};


#endif  // CONSTANTNUMBERPARSER_H
