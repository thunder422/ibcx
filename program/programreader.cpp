/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "programreader.h"
#include "programunit.h"


ProgramReader::ProgramReader(ProgramConstIterator begin, unsigned offset, unsigned size) :
    begin_iterator {begin},
    iterator {begin + offset},
    end_iterator {iterator + size}
{
}

Code *ProgramReader::getInstruction()
{
    return (*iterator++).instructionCode();
}

WordType ProgramReader::getOperand()
{
    return (*iterator++).operand();
}

unsigned ProgramReader::currentOffset() const
{
    return std::distance(begin_iterator, iterator);
}

bool ProgramReader::hasMoreCode() const
{
    return iterator != end_iterator;
}
