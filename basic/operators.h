/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_OPERATORS_H
#define IBC_OPERATORS_H

#include "code.h"
#include "codes.h"
#include "table.h"


enum class OpType {
    Dbl,
    Int,
    DblDbl,
    IntDbl,
    DblInt,
    IntInt,
    StrStr,
    StrTmp,
    TmpStr
};


template <OpType op_type>
class OperatorCode : public Code {
public:
    OperatorCode(RecreateFunctionPointer recreate_function,
            ExecuteFunctionPointer execute_function) :
        Code(recreate_function, execute_function) { }
};


class OperatorCodes : public Codes {
public:
    struct Info {
        Code &code;
        DataType result_data_type;
    };

    virtual Info select(DataType lhs_data_type, DataType rhs_data_type = {}) const = 0;
};

class UnaryOperatorCodes : public OperatorCodes {
public:
    UnaryOperatorCodes(Precedence precedence, const char *keyword,
        OperatorCode<OpType::Dbl> &dbl_code, OperatorCode<OpType::Int> &int_code);
    Info select(DataType data_type, DataType unused_data_type) const override;
    std::vector<WordType> codeValues() const override;

private:
    OperatorCode<OpType::Dbl> &dbl_code;
    OperatorCode<OpType::Int> &int_code;
};


struct NumCodes {
    NumCodes(OperatorCode<OpType::DblDbl> &dbl_dbl_code, OperatorCode<OpType::IntDbl> &int_dbl_code,
        OperatorCode<OpType::DblInt> &dbl_int_code, OperatorCode<OpType::IntInt> &int_int_code);
    OperatorCodes::Info select(DataType lhs_data_type, DataType rhs_data_type) const;

    OperatorCode<OpType::DblDbl> &dbl_dbl_code;
    OperatorCode<OpType::IntDbl> &int_dbl_code;
    OperatorCode<OpType::DblInt> &dbl_int_code;
    OperatorCode<OpType::IntInt> &int_int_code;
};


class NumOperatorCodes : public OperatorCodes {
public:
    NumOperatorCodes(Precedence precedence, const char *keyword,
        OperatorCode<OpType::DblDbl> &dbl_dbl_code, OperatorCode<OpType::IntDbl> &int_dbl_code,
        OperatorCode<OpType::DblInt> &dbl_int_code, OperatorCode<OpType::IntInt> &int_int_code);
    Info select(DataType lhs_data_type, DataType rhs_data_type) const override;
    std::vector<WordType> codeValues() const override;

private:
    NumCodes num_codes;
};


class IntDivOperatorCode : public OperatorCodes {
public:
    IntDivOperatorCode(Precedence precedence, const char *keyword,
        OperatorCode<OpType::DblDbl> &code);
    Info select(DataType lhs_data_type, DataType rhs_data_type) const override;
    std::vector<WordType> codeValues() const override;

private:
    OperatorCode<OpType::DblDbl> &code;
};


struct StrCodes {
    StrCodes(OperatorCode<OpType::StrStr> &str_str_code, OperatorCode<OpType::TmpStr> &tmp_str_code,
        OperatorCode<OpType::StrTmp> &str_tmp_code);
    OperatorCodes::Info select(DataType lhs_data_type, DataType rhs_data_type) const;

    OperatorCode<OpType::StrStr> &str_str_code;
    OperatorCode<OpType::TmpStr> &tmp_str_code;
    OperatorCode<OpType::StrTmp> &str_tmp_code;
};


class NumStrOperatorCodes : public OperatorCodes {
public:
    NumStrOperatorCodes(Precedence precedence, const char *keyword,
        OperatorCode<OpType::DblDbl> &dbl_dbl_code, OperatorCode<OpType::IntDbl> &int_dbl_code,
        OperatorCode<OpType::DblInt> &dbl_int_code, OperatorCode<OpType::IntInt> &int_int_code,
        OperatorCode<OpType::StrStr> &str_str_code, OperatorCode<OpType::TmpStr> &tmp_str_code,
        OperatorCode<OpType::StrTmp> &str_tmp_code);
    std::vector<WordType> codeValues() const;
    Info select(DataType lhs_data_type, DataType rhs_data_type) const override;

private:
    NumCodes num_codes;
    StrCodes str_codes;
};


class CompOperatorCodes : public NumStrOperatorCodes {
public:
    using NumStrOperatorCodes::NumStrOperatorCodes;
    Info select(DataType lhs_data_type, DataType rhs_data_type) const override;
};


class NotOperatorCodes : public OperatorCodes {
public:
    NotOperatorCodes(Precedence precedence, const char *keyword, OperatorCode<OpType::Int> &code);
    Info select(DataType unused_data_type1, DataType unused_data_type2) const override;
    std::vector<WordType> codeValues() const override;

private:
    OperatorCode<OpType::Int> &code;
};


class LogicOperatorCodes : public OperatorCodes {
public:
    LogicOperatorCodes(Precedence precedence, const char *keyword,
        OperatorCode<OpType::IntInt> &code);
    Info select(DataType unused_data_type1, DataType unused_data_type2) const override;
    std::vector<WordType> codeValues() const override;

private:
    OperatorCode<OpType::IntInt> &code;
};


struct ExpNumOperandError { };
struct ExpStrOperandError { };


#endif  // IBC_OPERATORS_H
