/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "commandcode.h"
#include "compiler.h"
#include "executer.h"
#include "programcode.h"
#include "recreator.h"


void compileEnd(Compiler &compiler);
void recreateEnd(Recreator &recreator);
void executeEnd(Executer &executer);

CommandCode end_code {"END", compileEnd, recreateEnd, executeEnd};


void compileEnd(Compiler &compiler)
{
    compiler.addInstruction(end_code);
}

void recreateEnd(Recreator &recreator)
{
    recreator.addCommandKeyword(end_code);
}

void executeEnd(Executer &executer)
{
    (void)executer;
    throw EndOfProgram {};
}
