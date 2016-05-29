/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>

#include "commandcode.h"
#include "compiler.h"
#include "expressioncompiler.h"
#include "programcode.h"
#include "recreator.h"


void print_compile(Compiler &compiler);
void print_recreate(Recreator &recreator);
void print_item_recreate(Recreator &recreator);

CommandCode print_code {"PRINT", print_compile, print_recreate};
Code print_dbl_code {print_item_recreate};
Code print_int_code {print_item_recreate};


void print_compile(Compiler &compiler)
{
    if (compiler.peekNextChar() != EOF) {
        auto data_type = ExpressionCompiler{compiler}(DataType::Null);
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
        recreator.push(print_code.getKeyword());
    } else {
        auto operand = recreator.top();
        recreator.pop();
        recreator.push(print_code.getKeyword());
        recreator.topAddSpace();
        recreator.topAdd(operand);
    }
}

void print_item_recreate(Recreator &recreator)
{
    (void)recreator;
}
