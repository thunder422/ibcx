/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "programcode.h"

ProgramCode::ProgramCode()
{
}

void ProgramCode::append(ProgramCode &more)
{
    code.insert(code.end(), more.code.begin(), more.code.end());
}
