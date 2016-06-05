/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "commandcode.h"


std::map<ci_string, CommandCode *> CommandCode::command_codes;
std::map<WordType, const char *> CommandCode::command_names;
std::map<WordType, std::function<void(Compiler &)>> CommandCode::compile_functions;

CommandCode *CommandCode::find(const ci_string &keyword)
{
    // FIXME need to check if keyword is present (this line will add a null code pointer)
    return command_codes[keyword];
}

void CommandCode::compile(Compiler &compiler) const
{
    compile_functions[getValue()](compiler);
}

const char *CommandCode::getKeyword() const
{
    return command_names[getValue()];
}


CommandCode::CommandCode(const char *keyword, std::function<void (Compiler &)> compile_function,
        std::function<void (Recreator &)> recreate_function) :
    Code {recreate_function, nullptr}
{
    command_codes[keyword] = this;
    command_names[getValue()] = keyword;
    compile_functions[getValue()] = compile_function;
}
