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
#include "token.h"

class ProgramCode;
class State;

class ConstNumParser {
public:
    ConstNumParser(std::istream &is_);

    UniqueToken getToken();
    DataType getCode(ProgramCode &code);
    void changeState(State *);
    void add(char c);
    void setDone();

private:
    void processInput();

    std::istream &is;
    State *state;
    std::string number;
    bool done;
};


#endif  // CONSTANTNUMBERPARSER_H
