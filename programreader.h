/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_PROGRAMREADER_H
#define IBC_PROGRAMREADER_H

#include "programcode.h"


class ProgramUnit;

class ProgramReader {
public:
    ProgramReader(ProgramConstIterator begin);
    Code *getInstruction();
    uint16_t getOperand();

private:
    ProgramConstIterator iterator;
};


#endif  // IBC_PROGRAMREADER_H
