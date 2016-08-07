/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "operators.h"


UnaryOperatorCodes::UnaryOperatorCodes(Precedence::Level precedence, const char *keyword,
        OperatorCode<OpType::Dbl> &dbl_code, OperatorCode<OpType::Int> &int_code) :
    dbl_code {dbl_code},
    int_code {int_code}
{
    Precedence::addOperatorCodes(precedence, *this, keyword);
}

OperatorInfo UnaryOperatorCodes::select(DataType data_type, DataType unused_data_type) const
{
    (void)unused_data_type;
    if (data_type == DataType::Double) {
        return OperatorInfo {dbl_code, DataType::Double};
    } else {
        return OperatorInfo {int_code, DataType::Integer};
    }
}

std::vector<WordType> UnaryOperatorCodes::codeValues() const
{
    return std::vector<WordType> {dbl_code.getValue(), int_code.getValue()};
}


NumOperatorCodes::NumOperatorCodes(Precedence::Level precedence, const char *keyword,
        OperatorCode<OpType::DblDbl> &dbl_dbl_code, OperatorCode<OpType::IntDbl> &int_dbl_code,
        OperatorCode<OpType::DblInt> &dbl_int_code, OperatorCode<OpType::IntInt> &int_int_code) :
    dbl_dbl_code {dbl_dbl_code},
    int_dbl_code {int_dbl_code},
    dbl_int_code {dbl_int_code},
    int_int_code {int_int_code}
{
    Precedence::addOperatorCodes(precedence, *this, keyword);
}

OperatorInfo NumOperatorCodes::select(DataType lhs_data_type, DataType rhs_data_type) const
{
    if (lhs_data_type == DataType::Integer && rhs_data_type == DataType::Integer) {
        return OperatorInfo {int_int_code, DataType::Integer};
    } else if (lhs_data_type == DataType::Integer) {
        return OperatorInfo {int_dbl_code, DataType::Double};
    } else if (rhs_data_type == DataType::Integer) {
        return OperatorInfo {dbl_int_code, DataType::Double};
    } else {
        return OperatorInfo {dbl_dbl_code, DataType::Double};
    }
}

std::vector<WordType> NumOperatorCodes::codeValues() const
{
    return std::vector<WordType> {
        dbl_dbl_code.getValue(), int_dbl_code.getValue(),
        dbl_int_code.getValue(), int_int_code.getValue()
    };
}


OperatorInfo CompOperatorCodes::select(DataType lhs_data_type, DataType rhs_data_type) const
{
    auto info = NumOperatorCodes::select(lhs_data_type, rhs_data_type);
    info.result_data_type = DataType::Integer;
    return info;
}


IntDivOperatorCode::IntDivOperatorCode(Precedence::Level precedence, const char *keyword,
        OperatorCode<OpType::DblDbl> &code) :
    code {code}
{
    Precedence::addOperatorCodes(precedence, *this, keyword);
}

OperatorInfo IntDivOperatorCode::select(DataType lhs_data_type, DataType rhs_data_type) const
{
    (void)lhs_data_type;
    (void)rhs_data_type;
    return OperatorInfo {code, DataType::Integer};
}

std::vector<WordType> IntDivOperatorCode::codeValues() const
{
    return std::vector<WordType> {code.getValue()};
}


NotOperatorCodes::NotOperatorCodes(Precedence::Level precedence, const char *keyword,
        OperatorCode<OpType::Int> &code) :
    code {code}
{
    Precedence::addOperatorCodes(precedence, *this, keyword);
}

OperatorInfo NotOperatorCodes::select(DataType unused_data_type1, DataType unused_data_type2) const
{
    (void)unused_data_type1;
    (void)unused_data_type2;
    return OperatorInfo {code, DataType::Integer};
}

std::vector<WordType> NotOperatorCodes::codeValues() const
{
    return std::vector<WordType> {code.getValue()};
}


LogicOperatorCodes::LogicOperatorCodes(Precedence::Level precedence, const char *keyword,
        OperatorCode<OpType::IntInt> &code) :
    code {code}
{
    Precedence::addOperatorCodes(precedence, *this, keyword);
}

OperatorInfo LogicOperatorCodes::select(DataType unused_data_type1, DataType unused_data_type2)
    const
{
    (void)unused_data_type1;
    (void)unused_data_type2;
    return OperatorInfo {code, DataType::Integer};
}

std::vector<WordType> LogicOperatorCodes::codeValues() const
{
    return std::vector<WordType> {code.getValue()};
}
