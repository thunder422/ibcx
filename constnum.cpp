/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "code.h"
#include "constnum.h"
#include "recreator.h"


void const_num_recreate(Recreator &recreator)
{
    auto number = recreator.getConstNumOperand();
    recreator.push(number);
}

Code const_dbl_code {const_num_recreate};
Code const_int_code {const_num_recreate};

ConstNumInfo ConstNumDictionary::add(bool floating_point, const std::string &number)
{
    ConstNumInfo const_num_info;
    if (!floating_point) {
        int int_value = std::stoi(number);
        const_num_info.code_value = const_int_code.getValue();
        const_num_info.data_type = DataType::Integer;
        int_values.push_back(int_value);
    } else {
        const_num_info.code_value = const_dbl_code.getValue();
        const_num_info.data_type = DataType::Double;
    }
    const_num_info.operand = Dictionary::add(number);
    return const_num_info;
}
