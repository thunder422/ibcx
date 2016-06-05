/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "code.h"
#include "constnum.h"
#include "executer.h"
#include "recreator.h"


void const_num_recreate(Recreator &recreator)
{
    auto number = recreator.getConstNumOperand();
    recreator.push(number);
}

void const_dbl_execute(Executer &executer)
{
    auto operand = executer.getOperand();
    executer.pushConstDbl(operand);
}

void const_int_execute(Executer &executer)
{
    auto operand = executer.getOperand();
    executer.pushConstInt(operand);
}

Code const_dbl_code {const_num_recreate, const_dbl_execute};
Code const_int_code {const_num_recreate, const_int_execute};

ConstNumInfo ConstNumDictionary::add(bool floating_point, const std::string &number)
{
    ConstNumInfo const_num_info;
    double dbl_value;
    int int_value;
    if (!floating_point) {
        const_num_info.code_value = const_int_code.getValue();
        const_num_info.data_type = DataType::Integer;
        int_value = std::stoi(number);
        dbl_value = int_value;
    } else {
        const_num_info.code_value = const_dbl_code.getValue();
        const_num_info.data_type = DataType::Double;
        dbl_value = std::stod(number);
        int_value = dbl_value;
    }
    dbl_values.push_back(dbl_value);
    int_values.push_back(int_value);
    const_num_info.operand = Dictionary::add(number);
    return const_num_info;
}
