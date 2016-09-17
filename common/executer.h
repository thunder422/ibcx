/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_EXECUTER_H
#define IBC_EXECUTER_H

#include <iosfwd>
#include <memory>
#include <random>
#include <stack>

#include "code.h"
#include "wordtype.h"


class Executer {
public:
    struct StackItem {
        StackItem(double dbl_value);
        StackItem(int32_t int_value);
        StackItem(std::string *str_value);

        union {
            double dbl_value;
            int32_t int_value;
            std::string *str_value;
        };
    };

    Executer(const WordType *code, const double *const_dbl_values, const int32_t *const_int_values,
        const std::unique_ptr<std::string> *const_str_values, std::ostream &os);
    void run();
    void executeOneCode();
    unsigned currentOffset() const;

    WordType getOperand();
    template <typename T> void push(T value);
    void pushConstDbl(WordType operand);
    void pushConstInt(WordType operand);
    void pushConstStr(WordType operand);
    double topDbl() const;
    int32_t topInt() const;
    const std::string *topStr() const;
    std::string *topTmpStr() const;
    template <typename T> T top() const;
    double topIntAsDbl() const;
    void pop();
    void setTop(double value);
    void setTop(int32_t value);
    void setTopIntFromInt64(int64_t value);
    void setTopIntFromDouble(double value);
    void setTopIntFromBool(bool value);
    void setTop(std::string *value);
    std::ostream &output();
    bool stackEmpty() const;
    double getRandomNumber();
    int32_t getRandomNumber(int32_t limit);

private:
    void reset();

    const WordType *code;
    const ExecuteFunctionPointer *execute_functions;
    const double *const_dbl_values;
    const int32_t *const_int_values;
    const std::unique_ptr<std::string> *const_str_values;

    WordType *program_counter;
    std::stack<StackItem> stack;
    std::ostream &os;
    std::uniform_real_distribution<double> uniform_distribution {0.0, 1.0};
};

inline WordType Executer::getOperand()
{
    return *program_counter++;
}

template <typename T>
inline void Executer::push(T value)
{
    stack.emplace(value);
}

inline void Executer::pushConstDbl(WordType operand)
{
    stack.emplace(const_dbl_values[operand]);
}

inline void Executer::pushConstInt(WordType operand)
{
    stack.emplace(const_int_values[operand]);
}

inline void Executer::pushConstStr(WordType operand)
{
    stack.emplace(const_str_values[operand].get());
}

inline double Executer::topDbl() const
{
    return stack.top().dbl_value;
}

inline int32_t Executer::topInt() const
{
    return stack.top().int_value;
}

inline const std::string *Executer::topStr() const
{
    return stack.top().str_value;
}

inline std::string *Executer::topTmpStr() const
{
    return stack.top().str_value;
}

template <>
inline double Executer::top() const
{
    return topDbl();
}

template <>
inline int32_t Executer::top() const
{
    return topInt();
}

inline double Executer::topIntAsDbl() const
{
    return static_cast<double>(topInt());
}

inline void Executer::pop()
{
    stack.pop();
}

inline void Executer::setTop(double value)
{
    stack.top().dbl_value = value;
}

inline void Executer::setTop(int32_t value)
{
    stack.top().int_value = value;
}

inline void Executer::setTopIntFromInt64(int64_t value)
{
    stack.top().int_value = static_cast<int32_t>(value);
}

inline void Executer::setTopIntFromDouble(double value)
{
    stack.top().int_value = static_cast<int32_t>(value);
}

inline void Executer::setTopIntFromBool(bool value)
{
    stack.top().int_value = value ? -1 : 0;
}

inline void Executer::setTop(std::string *value)
{
    stack.top().str_value = value;
}


inline Executer::StackItem::StackItem(double dbl_value) :
    dbl_value {dbl_value}
{
}

inline Executer::StackItem::StackItem(int32_t int_value) :
    int_value {int_value}
{
}

inline Executer::StackItem::StackItem(std::string *str_value) :
    str_value {str_value}
{
}


struct EndOfProgram { };


#endif  // IBC_EXECUTER_H
