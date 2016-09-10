/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>

#include "code.h"
#include "executer.h"


std::default_random_engine random_number_generator;


Executer::Executer(const WordType *code, const double *const_dbl_values,
        const int32_t *const_int_values, const std::unique_ptr<std::string> *const_str_values,
        std::ostream &os) :
    code {code},
    execute_functions {Code::getExecuteFunctions()},
    const_dbl_values {const_dbl_values},
    const_int_values {const_int_values},
    const_str_values {const_str_values},
    os {os}
{
    reset();
}

void Executer::run()
{
    reset();
    for (;;) {
        executeOneCode();
    }
}

void Executer::reset()
{
    program_counter = const_cast<WordType *>(code);
}

void Executer::executeOneCode()
{
    auto code_value = *program_counter++;
    execute_functions[code_value](*this);
}

unsigned Executer::currentOffset() const
{
    return program_counter - code - 1;
}

std::ostream &Executer::output()
{
    return os;
}

bool Executer::stackEmpty() const
{
    return stack.empty();
}

double Executer::getRandomNumber()
{
    return uniform_distribution(random_number_generator);
}

int32_t Executer::getRandomNumber(int32_t limit)
{
    return std::uniform_int_distribution<int>{1, limit}(random_number_generator);
}
