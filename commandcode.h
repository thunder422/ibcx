/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_COMMANDCODE_H
#define IBC_COMMANDCODE_H

#include <unordered_map>

#include <code.h>


class Compiler;
class ProgramCode;
class ProgramUnit;

class CommandCode : public Code {
public:
    static CommandCode *find(const std::string &keyword);

    CommandCode(const char *keyword);
    virtual void compile(Compiler &compiler) const = 0;

private:
    static std::unordered_map<std::string, CommandCode *> command_names;
};


#endif  // IBC_COMMANDCODE_H
