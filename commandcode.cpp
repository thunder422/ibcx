/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "commandcode.h"


std::map<ci_string, CommandCode *> CommandCode::command_codes;
std::map<uint16_t, const char *> CommandCode::command_names;

CommandCode *CommandCode::find(const ci_string &keyword)
{
    return command_codes.find(keyword)->second;
}


CommandCode::CommandCode(const char *keyword)
{
    command_codes.emplace(keyword, this);
    command_names.emplace(getValue(), keyword);
}

const char *CommandCode::getKeyword() const
{
    return command_names.find(getValue())->second;
}
