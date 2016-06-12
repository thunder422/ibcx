/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "commandcode.h"


std::map<ci_string, CommandCode *> &CommandCode::commandCodes()
{
    static std::map<ci_string, CommandCode *> command_codes;
    return command_codes;
}

std::map<WordType, const char *> &CommandCode::commandNames()
{
    static std::map<WordType, const char *> command_names;
    return command_names;
}

std::map<WordType, CompilerFunctionPointer> &CommandCode::compileFunctions()
{
    static std::map<WordType, CompilerFunctionPointer> compile_functions;
    return compile_functions;
}

CommandCode *CommandCode::find(const ci_string &keyword)
{
    // FIXME need to check if keyword is present (this line will add a null code pointer)
    return commandCodes()[keyword];
}

void CommandCode::compile(Compiler &compiler) const
{
    compileFunctions()[getValue()](compiler);
}

const char *CommandCode::getKeyword() const
{
    return commandNames()[getValue()];
}


CommandCode::CommandCode(const char *keyword, CompilerFunctionPointer compile_function,
        RecreateFunctionPointer recreate_function, ExecuteFunctionPointer execute_function) :
    Code {recreate_function, execute_function}
{
    commandCodes()[keyword] = this;
    commandNames()[getValue()] = keyword;
    compileFunctions()[getValue()] = compile_function;
}
