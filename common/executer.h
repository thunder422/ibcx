/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_EXECUTER_H
#define IBC_EXECUTER_H

#include <iosfwd>
#include <stack>

#include "wordtype.h"


class Executer {
public:
    struct StackItem {
        StackItem(double dbl_value);
        StackItem(int int_value);

        union {
            double dbl_value;
            int int_value;
        };
    };

    Executer(const WordType *code, const double *const_dbl_values, const int *const_int_values,
        std::ostream &os);
    void run();
    void executeOneCode();

    WordType getOperand();
    void pushConstDbl(WordType operand);
    void pushConstInt(WordType operand);
    StackItem &top();
    void pop();
    std::ostream &output();
    bool stackEmpty() const;

private:
    void reset();

    const WordType *code;
    const ExecuteFunctionPointer *execute_functions;
    const double *const_dbl_values;
    const int *const_int_values;

    WordType *program_counter;
    std::stack<StackItem> stack;
    std::ostream &os;
};

inline WordType Executer::getOperand()
{
    return *program_counter++;
}

inline void Executer::pushConstDbl(WordType operand)
{
    stack.emplace(const_dbl_values[operand]);
}

inline void Executer::pushConstInt(WordType operand)
{
    stack.emplace(const_int_values[operand]);
}

inline Executer::StackItem &Executer::top()
{
    return stack.top();
}

inline void Executer::pop()
{
    stack.pop();
}


inline Executer::StackItem::StackItem(double dbl_value) :
    dbl_value {dbl_value}
{
}

inline Executer::StackItem::StackItem(int int_value) :
    int_value {int_value}
{
}


struct EndOfProgram { };


#endif  // IBC_EXECUTER_H