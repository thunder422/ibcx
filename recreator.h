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


class ProgramUnit;

class Recreator {
public:
    Recreator(ProgramUnit &program);
    void recreateOneCode();
    std::string getConstNumOperand();
    void push(const std::string &operand);
    std::string top() const;

private:
    struct StackItem {
        StackItem(const std::string &string);

        std::string string;
    };

    ProgramUnit &program;
    ProgramReader program_reader;
    std::stack<StackItem> stack;
};

inline Recreator::StackItem::StackItem(const std::string &string) :
    string {string}
{
}


#endif  // IBC_RECREATOR_H
