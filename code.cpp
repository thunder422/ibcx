/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "code.h"


std::vector<Code *> Code::codes;
std::vector<std::function<void(Recreator &)>> Code::recreate_functions;

Code::type Code::addCode(Code *code)
{
    type value = codes.size();
    codes.emplace_back(code);
    return value;
}

Code *Code::getCode(Code::type value)
{
    return codes[value];
}


Code::Code(std::function<void(Recreator &)> recreate_function) :
    value {addCode(this)}
{
    recreate_functions.emplace_back(recreate_function);
}

void Code::recreate(Recreator &recreator) const
{
    recreate_functions[value](recreator);
}
