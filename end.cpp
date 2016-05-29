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


class EndCode : public CommandCode {
public:
    EndCode(std::function<void(Recreator &)> recreate_function);
    void compile(Compiler &compiler) const override;
};

EndCode::EndCode(std::function<void(Recreator &)> recreate_function) :
    CommandCode {recreate_function, "END"}
{
}


void end_recreate(Recreator &recreator);

EndCode end_code(end_recreate);


void EndCode::compile(Compiler &compiler) const
{
    compiler.addInstruction(end_code);
}

void end_recreate(Recreator &recreator)
{
    recreator.push(end_code.getKeyword());
}
