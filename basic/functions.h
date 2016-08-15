/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_FUNCTIONS_H
#define IBC_FUNCTIONS_H

#include "code.h"
#include "codes.h"
#include "table.h"


enum class ArgType {
    Dbl
};


template <ArgType arg_type>
class FunctionCode : public Code {
public:
    FunctionCode(RecreateFunctionPointer recreate_function,
            ExecuteFunctionPointer execute_function) :
        Code(recreate_function, execute_function) { }
};


class MultiTypeFunctionCodes : public Codes {
public:
    MultiTypeFunctionCodes(const char *keyword, FunctionCode<ArgType::Dbl> &dbl_code);
    Info select(DataType data_type_1, DataType unused_data_type) const override;
    std::vector<WordType> codeValues() const override;

private:
    FunctionCode<ArgType::Dbl> &dbl_code;
};

#endif  // IBC_FUNCTIONS_H
