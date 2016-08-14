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
#include "precedence.h"


enum class OpType {
    Dbl,
    Int,
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


class UnaryOperatorCodes : public Codes {
public:
    UnaryOperatorCodes(Precedence::Level precedence, const char *keyword,
        OperatorCode<OpType::Dbl> &dbl_code, OperatorCode<OpType::Int> &int_code);
    Info select(DataType data_type, DataType unused_data_type) const override;
    std::vector<WordType> codeValues() const override;

private:
    OperatorCode<OpType::Dbl> &dbl_code;
    OperatorCode<OpType::Int> &int_code;
};


class NumOperatorCodes : public Codes {
public:
    NumOperatorCodes(Precedence::Level precedence, const char *keyword,
        OperatorCode<OpType::DblDbl> &dbl_dbl_code, OperatorCode<OpType::IntDbl> &int_dbl_code,
        OperatorCode<OpType::DblInt> &dbl_int_code, OperatorCode<OpType::IntInt> &int_int_code);
    Info select(DataType lhs_data_type, DataType rhs_data_type) const override;
    std::vector<WordType> codeValues() const override;

private:
    OperatorCode<OpType::DblDbl> &dbl_dbl_code;
    OperatorCode<OpType::IntDbl> &int_dbl_code;
    OperatorCode<OpType::DblInt> &dbl_int_code;
    OperatorCode<OpType::IntInt> &int_int_code;
};


class IntDivOperatorCode : public Codes {
public:
    IntDivOperatorCode(Precedence::Level precedence, const char *keyword,
        OperatorCode<OpType::DblDbl> &code);
    Info select(DataType lhs_data_type, DataType rhs_data_type) const override;
    std::vector<WordType> codeValues() const override;

private:
    OperatorCode<OpType::DblDbl> &code;
};


class CompOperatorCodes : public NumOperatorCodes {
public:
    using NumOperatorCodes::NumOperatorCodes;
    Info select(DataType lhs_data_type, DataType rhs_data_type) const override;
};


class NotOperatorCodes : public Codes {
public:
    NotOperatorCodes(Precedence::Level precedence, const char *keyword,
        OperatorCode<OpType::Int> &code);
    Info select(DataType unused_data_type1, DataType unused_data_type2) const override;
    std::vector<WordType> codeValues() const override;

private:
    OperatorCode<OpType::Int> &code;
};


class LogicOperatorCodes : public Codes {
public:
    LogicOperatorCodes(Precedence::Level precedence, const char *keyword,
        OperatorCode<OpType::IntInt> &code);
    Info select(DataType unused_data_type1, DataType unused_data_type2) const override;
    std::vector<WordType> codeValues() const override;

private:
    OperatorCode<OpType::IntInt> &code;
};


#endif  // IBC_OPERATORS_H
