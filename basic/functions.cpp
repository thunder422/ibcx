/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "functions.h"


MultiTypeFunctionCodes::MultiTypeFunctionCodes(const char *keyword,
        FunctionCode<ArgType::Dbl> &dbl_code) :
    dbl_code {dbl_code}
{
    Table::addNumFunctionCodes(*this, keyword);
}

Codes::Info MultiTypeFunctionCodes::select(DataType data_type_1, DataType unused_data_type) const
{
    (void)unused_data_type;
    (void)data_type_1;
    return Codes::Info {dbl_code, DataType::Double};
}

std::vector<WordType> MultiTypeFunctionCodes::codeValues() const
{
    return std::vector<WordType> {dbl_code.getValue()};
}
