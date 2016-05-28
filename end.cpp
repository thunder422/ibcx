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
    EndCode();
    void compile(Compiler &compiler) const override;
    void recreate(Recreator &recreator) const override;
};

EndCode end_code;

EndCode::EndCode() :
    CommandCode("END")
{
}

void EndCode::compile(Compiler &compiler) const
{
    compiler.addInstruction(end_code);
}

void EndCode::recreate(Recreator &recreator) const
{
    recreator.push(getKeyword());
}
