/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>

#include "commandcode.h"
#include "compiler.h"
#include "executer.h"
#include "programcode.h"
#include "recreator.h"


void compilePrint(Compiler &compiler);
void recreatePrint(Recreator &recreator);
void recreatePrintItem(Recreator &recreator);
void executePrint(Executer &executer);
void executePrintInt(Executer &executer);
void executePrintDbl(Executer &executer);

CommandCode print_code {"PRINT", compilePrint, recreatePrint, executePrint};
Code print_dbl_code {recreatePrintItem, executePrintDbl};
Code print_int_code {recreatePrintItem, executePrintInt};
Code print_str_code {nullptr, nullptr};


void compilePrint(Compiler &compiler)
{
    if (compiler.peekNextChar() != EOF) {
        auto data_type = compiler.compileExpression();
        if (data_type.isDouble()) {
            compiler.addInstruction(print_dbl_code);
        } else if (data_type.isInteger()) {
            compiler.addInstruction(print_int_code);
        } else {
            compiler.addInstruction(print_str_code);
        }
    }
    compiler.addInstruction(print_code);
}

void recreatePrint(Recreator &recreator)
{
    recreator.addCommandKeyword(print_code);
}

void recreatePrintItem(Recreator &recreator)
{
    (void)recreator;
}


void executePrint(Executer &executer)
{
    executer.output() << std::endl;
}

void executePrintInt(Executer &executer)
{
    executer.output() << executer.topInt();
    executer.pop();
}

void executePrintDbl(Executer &executer)
{
    executer.output() << executer.topDbl();
    executer.pop();
}
