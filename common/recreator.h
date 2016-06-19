/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_RECREATOR_H
#define IBC_RECREATOR_H

#include <stack>
#include <string>

#include "programreader.h"


class CommandCode;
class ProgramUnit;

class Recreator {
public:
    Recreator(ProgramUnit &program, ProgramReader program_reader);
    std::string operator()();
    std::string getConstNumOperand();
    void pushKeyword(CommandCode command_code);
    void push(const std::string &operand);
    bool empty() const;
    std::string top() const;
    void pop();
    void append(char c);
    void append(const std::string &string);
    void swapTop(std::string &string);
    void prependKeyword(CommandCode command_code);

private:
    struct StackItem {
        StackItem(const std::string &string);

        std::string string;
    };

    void recreateOneCode();

    ProgramUnit &program;
    ProgramReader program_reader;
    std::stack<StackItem> stack;
};

inline Recreator::StackItem::StackItem(const std::string &string) :
    string {string}
{
}


#endif  // IBC_RECREATOR_H
