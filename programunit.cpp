/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "programreader.h"
#include "programunit.h"

ProgramUnit::ProgramUnit()
{
}

void ProgramUnit::addCodeLine(ProgramCode &code_line)
{
    code.append(code_line);
}

ProgramReader ProgramUnit::createProgramReader() const
{
    return ProgramReader {code.begin()};
}
