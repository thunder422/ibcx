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


class PrintCode : public CommandCode {
public:
    PrintCode();
    void compile(Compiler &compiler) const override;
};

PrintCode print_code;
Code print_dbl_code;
Code print_int_code;

PrintCode::PrintCode() :
    CommandCode("PRINT")
{
}

void PrintCode::compile(Compiler &compiler) const
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
