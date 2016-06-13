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


void print_compile(Compiler &compiler);
void print_recreate(Recreator &recreator);
void print_item_recreate(Recreator &recreator);
void print_execute(Executer &executer);
void print_int_execute(Executer &executer);
void print_dbl_execute(Executer &executer);

CommandCode print_code {"PRINT", print_compile, print_recreate, print_execute};
Code print_dbl_code {print_item_recreate, print_dbl_execute};
Code print_int_code {print_item_recreate, print_int_execute};


void print_compile(Compiler &compiler)
{
    if (compiler.peekNextChar() != EOF) {
        auto data_type = compiler.compileExpression(DataType::Null);
        if (data_type == DataType::Double) {
            compiler.addInstruction(print_dbl_code);
        } else {
            compiler.addInstruction(print_int_code);
        }
    }
    compiler.addInstruction(print_code);
}

void print_recreate(Recreator &recreator)
{
    if (recreator.empty()) {
        recreator.pushKeyword(print_code);
    } else {
        recreator.prependKeyword(print_code);
    }
}

void print_item_recreate(Recreator &recreator)
{
    (void)recreator;
}


void print_execute(Executer &executer)
{
    executer.output() << std::endl;
}

void print_int_execute(Executer &executer)
{
    executer.output() << executer.top().int_value;
    executer.pop();
}

void print_dbl_execute(Executer &executer)
{
    executer.output() << executer.top().dbl_value;
    executer.pop();
}
