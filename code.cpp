/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "code.h"


std::vector<Code *> Code::codes;
std::vector<std::function<void(Recreator &)>> Code::recreate_functions;
std::vector<ExecuteFunctionPointer> Code::execute_functions;

WordType Code::addCode(Code *code)
{
    WordType value = codes.size();
    codes.emplace_back(code);
    return value;
}

Code *Code::getCode(WordType value)
{
    return codes[value];
}


Code::Code(std::function<void(Recreator &)> recreate_function,
        ExecuteFunctionPointer execute_function) :
    value {addCode(this)}
{
    recreate_functions.emplace_back(recreate_function);
    execute_functions.emplace_back(execute_function);
}

void Code::recreate(Recreator &recreator) const
{
    recreate_functions[value](recreator);
}

const ExecuteFunctionPointer *Code::getExecuteFunctions()
{
    return execute_functions.data();
}
