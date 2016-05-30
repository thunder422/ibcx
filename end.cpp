/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "commandcode.h"
#include "compiler.h"
#include "programcode.h"
#include "recreator.h"


void end_compile(Compiler &compiler);
void end_recreate(Recreator &recreator);

CommandCode end_code {"END", end_compile, end_recreate};


void end_compile(Compiler &compiler)
{
    compiler.addInstruction(end_code);
}

void end_recreate(Recreator &recreator)
{
    recreator.pushKeyword(end_code);
}
