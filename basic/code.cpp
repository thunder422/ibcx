/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "code.h"


WordType Code::addCode(Code *code)
{
    WordType value = codes().size();
    codes().emplace_back(code);
    return value;
}

std::vector<Code *> &Code::codes()
{
    static std::vector<Code *> codes;
    return codes;
}

std::vector<RecreateFunctionPointer> &Code::recreateFunctions()
{
    static std::vector<RecreateFunctionPointer> recreate_functions;
    return recreate_functions;
}

std::vector<ExecuteFunctionPointer> &Code::executeFunctions()
{
    static std::vector<ExecuteFunctionPointer> execute_functions;
    return execute_functions;
}

Code *Code::getCode(WordType value)
{
    return codes()[value];
}


Code::Code(RecreateFunctionPointer recreate_function, ExecuteFunctionPointer execute_function) :
    value {addCode(this)}
{
    recreateFunctions().emplace_back(recreate_function);
    executeFunctions().emplace_back(execute_function);
}

void Code::recreate(Recreator &recreator) const
{
    recreateFunctions()[value](recreator);
}

const ExecuteFunctionPointer *Code::getExecuteFunctions()
{
    return executeFunctions().data();
}
