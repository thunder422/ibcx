/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "operators.h"


UnaryOperatorCodes::UnaryOperatorCodes(Precedence precedence, const char *keyword,
        OperatorCode<OpType::Dbl> &dbl_code, OperatorCode<OpType::Int> &int_code) :
    dbl_code {dbl_code},
    int_code {int_code}
{
    Table::addOperatorCodes(precedence, *this, keyword);
}

OperatorCodes::Info UnaryOperatorCodes::select(DataType data_type, DataType unused_data_type) const
{
    (void)unused_data_type;
    if (data_type.isDouble()) {
        return Info {dbl_code, DataType::Double()};
    } else {
        return Info {int_code, DataType::Integer()};
    }
}

std::vector<WordType> UnaryOperatorCodes::codeValues() const
{
    return std::vector<WordType> {dbl_code.getValue(), int_code.getValue()};
}


NumCodes::NumCodes(OperatorCode<OpType::DblDbl> &dbl_dbl_code,
        OperatorCode<OpType::IntDbl> &int_dbl_code, OperatorCode<OpType::DblInt> &dbl_int_code,
        OperatorCode<OpType::IntInt> &int_int_code) :
    dbl_dbl_code {dbl_dbl_code},
    int_dbl_code {int_dbl_code},
    dbl_int_code {dbl_int_code},
    int_int_code {int_int_code}
{
}

OperatorCodes::Info NumCodes::select(DataType lhs_data_type, DataType rhs_data_type,
    ErrorSide error_side) const
{
    if (lhs_data_type.isDouble()) {
        if (rhs_data_type.isDouble()) {
            return OperatorCodes::Info {dbl_dbl_code, DataType::Double()};
        } else if (rhs_data_type.isInteger()) {
            return OperatorCodes::Info {dbl_int_code, DataType::Double()};
        }
    } else if (lhs_data_type.isInteger()) {
        if (rhs_data_type.isDouble()) {
            return OperatorCodes::Info {int_dbl_code, DataType::Double()};
        } else if (rhs_data_type.isInteger()) {
            return OperatorCodes::Info {int_int_code, DataType::Integer()};
        }
    }
    if (error_side == ErrorSide::Left) {
        throw ExpNumLeftOperandError {};
    } else {
        throw ExpNumRightOperandError {};
    }
}


NumOperatorCodes::NumOperatorCodes(Precedence precedence, const char *keyword,
        OperatorCode<OpType::DblDbl> &dbl_dbl_code, OperatorCode<OpType::IntDbl> &int_dbl_code,
        OperatorCode<OpType::DblInt> &dbl_int_code, OperatorCode<OpType::IntInt> &int_int_code) :
    num_codes {dbl_dbl_code, int_dbl_code, dbl_int_code, int_int_code}
{
    Table::addOperatorCodes(precedence, *this, keyword);
}

OperatorCodes::Info NumOperatorCodes::select(DataType lhs_data_type, DataType rhs_data_type) const
{
    return num_codes.select(lhs_data_type, rhs_data_type, NumCodes::ErrorSide::Left);
}

std::vector<WordType> NumOperatorCodes::codeValues() const
{
    return std::vector<WordType> {
        num_codes.dbl_dbl_code.getValue(), num_codes.int_dbl_code.getValue(),
        num_codes.dbl_int_code.getValue(), num_codes.int_int_code.getValue()
    };
}


NumStrOperatorCodes::NumStrOperatorCodes(Precedence precedence, const char *keyword,
        OperatorCode<OpType::DblDbl> &dbl_dbl_code, OperatorCode<OpType::IntDbl> &int_dbl_code,
        OperatorCode<OpType::DblInt> &dbl_int_code, OperatorCode<OpType::IntInt> &int_int_code,
        OperatorCode<OpType::StrStr> &str_str_code) :
    num_codes {dbl_dbl_code, int_dbl_code, dbl_int_code, int_int_code},
    str_str_code {str_str_code}
{
    Table::addOperatorCodes(precedence, *this, keyword);
}

std::vector<WordType> NumStrOperatorCodes::codeValues() const
{
    return std::vector<WordType> {
        num_codes.dbl_dbl_code.getValue(), num_codes.int_dbl_code.getValue(),
        num_codes.dbl_int_code.getValue(), num_codes.int_int_code.getValue(),
        str_str_code.getValue()
    };
}

OperatorCodes::Info NumStrOperatorCodes::select(DataType lhs_data_type, DataType rhs_data_type)
    const
{
    if (lhs_data_type.isString()) {
        if (rhs_data_type.isString()) {
            return Info {str_str_code, DataType::TmpStr()};
        } else {
            throw ExpStrOperandError {};
        }
    } else {
        return num_codes.select(lhs_data_type, rhs_data_type, NumCodes::ErrorSide::Right);
    }
}


IntDivOperatorCode::IntDivOperatorCode(Precedence precedence, const char *keyword,
        OperatorCode<OpType::DblDbl> &code) :
    code {code}
{
    Table::addOperatorCodes(precedence, *this, keyword);
}

OperatorCodes::Info IntDivOperatorCode::select(DataType lhs_data_type, DataType rhs_data_type) const
{
    (void)lhs_data_type;
    (void)rhs_data_type;
    return Info {code, DataType::Integer()};
}

std::vector<WordType> IntDivOperatorCode::codeValues() const
{
    return std::vector<WordType> {code.getValue()};
}


NotOperatorCodes::NotOperatorCodes(Precedence precedence, const char *keyword,
        OperatorCode<OpType::Int> &code) :
    code {code}
{
    Table::addOperatorCodes(precedence, *this, keyword);
}

OperatorCodes::Info NotOperatorCodes::select(DataType unused_data_type1, DataType unused_data_type2)
    const
{
    (void)unused_data_type1;
    (void)unused_data_type2;
    return Info {code, DataType::Integer()};
}

std::vector<WordType> NotOperatorCodes::codeValues() const
{
    return std::vector<WordType> {code.getValue()};
}


LogicOperatorCodes::LogicOperatorCodes(Precedence precedence, const char *keyword,
        OperatorCode<OpType::IntInt> &code) :
    code {code}
{
    Table::addOperatorCodes(precedence, *this, keyword);
}

OperatorCodes::Info LogicOperatorCodes::select(DataType unused_data_type1,
    DataType unused_data_type2) const
{
    (void)unused_data_type1;
    (void)unused_data_type2;
    return Info {code, DataType::Integer()};
}

std::vector<WordType> LogicOperatorCodes::codeValues() const
{
    return std::vector<WordType> {code.getValue()};
}


OperatorCodes::Info CompOperatorCodes::select(DataType lhs_data_type, DataType rhs_data_type) const
{
    auto info = NumStrOperatorCodes::select(lhs_data_type, rhs_data_type);
    info.result_data_type = DataType::Integer();
    return info;
}
