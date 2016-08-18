/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "functions.h"


MultiTypeFunctionCodes::MultiTypeFunctionCodes(const char *keyword,
        FunctionCode<ArgType::Dbl> &dbl_code, FunctionCode<ArgType::Int> &int_code) :
    dbl_code {dbl_code},
    int_code {int_code}
{
    Table::addNumFunctionCodes(*this, keyword);
}

FunctionCodes::Info MultiTypeFunctionCodes::select(DataType data_type) const
{
    if (data_type == DataType::Double) {
        return FunctionCodes::Info {dbl_code, DataType::Double};
    } else {
        return FunctionCodes::Info {int_code, DataType::Integer};
    }
}

std::vector<WordType> MultiTypeFunctionCodes::codeValues() const
{
    return std::vector<WordType> {dbl_code.getValue(), int_code.getValue()};
}
