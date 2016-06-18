/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_OPERATORS_H
#define IBC_OPERATORS_H

#include "code.h"
#include "datatype.h"


enum class OpType {
    DblDbl,
    IntDbl,
    DblInt,
    IntInt
};


template <OpType op_type>
class OperatorCode : public Code {
public:
    OperatorCode(RecreateFunctionPointer recreate_function,
            ExecuteFunctionPointer execute_function) :
        Code(recreate_function, execute_function) { }
};


struct OperatorInfo {
    Code &code;
    DataType result_data_type;
};


class NumOperatorCodes {
public:
    NumOperatorCodes(OperatorCode<OpType::DblDbl> &dbl_dbl_code,
            OperatorCode<OpType::IntDbl> &int_dbl_code, OperatorCode<OpType::DblInt> &dbl_int_code,
            OperatorCode<OpType::IntInt> &int_int_code);
    OperatorInfo select(DataType lhs_data_type, DataType rhs_data_type) const;

private:
    OperatorCode<OpType::DblDbl> &dbl_dbl_code;
    OperatorCode<OpType::IntDbl> &int_dbl_code;
    OperatorCode<OpType::DblInt> &dbl_int_code;
    OperatorCode<OpType::IntInt> &int_int_code;
};


#endif  // IBC_OPERATORS_H
