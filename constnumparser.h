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
class ProgramModel;
class State;

class ConstNumParser {
public:
    ConstNumParser(std::istream &is_);

    DataType getCode(ProgramCode &code, ProgramModel &program);
    void changeState(State *);
    void addNextChar();
    void setDone();

private:
    void processInput();

    std::istream &is;
    State *state;
    std::string number;
    bool done;
};


#endif  // CONSTANTNUMBERPARSER_H
