/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "programreader.h"
#include "programunit.h"


ProgramReader::ProgramReader(ProgramConstIterator begin) :
    iterator {begin}
{
}

Code *ProgramReader::getInstruction()
{
    return (*iterator++).instructionCode();
}

uint16_t ProgramReader::getOperand()
{
    return (*iterator++).operand();
}
