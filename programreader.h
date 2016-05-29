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
    ProgramReader(ProgramConstIterator begin, unsigned offset, unsigned size);
    Code *getInstruction();
    uint16_t getOperand();
    bool hasMoreCode() const;

private:
    ProgramConstIterator iterator;
    ProgramConstIterator end_iterator;
};


#endif  // IBC_PROGRAMREADER_H
