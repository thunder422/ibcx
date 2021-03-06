/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "executer.h"
#include "operators.h"
#include "recreator.h"

using DblCompareFunction = bool(*)(double, double);
using IntCompareFunction = bool(*)(int32_t, int32_t);
using StrCompareFunction = bool(*)(const std::string *, const std::string *);

template <DblCompareFunction compare>
void executeCompareDblDbl(Executer &executer)
{
    auto rhs = executer.topDbl();
    executer.pop();
    executer.setTopIntFromBool(compare(executer.topDbl(), rhs));
}

template <DblCompareFunction compare>
void executeCompareIntDbl(Executer &executer)
{
    auto rhs = executer.topDbl();
    executer.pop();
    executer.setTopIntFromBool(compare(executer.topIntAsDbl(), rhs));
}

template <DblCompareFunction compare>
void executeCompareDblInt(Executer &executer)
{
    auto rhs = executer.topIntAsDbl();
    executer.pop();
    executer.setTopIntFromBool(compare(executer.topDbl(), rhs));
}

template <IntCompareFunction compare>
void executeCompareIntInt(Executer &executer)
{
    auto rhs = executer.topInt();
    executer.pop();
    executer.setTopIntFromBool(compare(executer.topInt(), rhs));
}

template <StrCompareFunction compare>
void executeCompareStrStr(Executer &executer)
{
    auto rhs = executer.topStr();
    executer.pop();
    auto lhs = executer.topStr();
    executer.setTopIntFromBool(compare(lhs, rhs));
}

template <StrCompareFunction compare>
void executeCompareTmpStr(Executer &executer)
{
    auto rhs = executer.topStr();
    executer.pop();
    auto lhs = executer.moveTopTmpStr();
    executer.setTopIntFromBool(compare(lhs.get(), rhs));
}

template <StrCompareFunction compare>
void executeCompareStrTmp(Executer &executer)
{
    auto rhs = executer.moveTopTmpStr();
    executer.pop();
    auto lhs = executer.topStr();
    executer.setTopIntFromBool(compare(lhs, rhs.get()));
}

template <StrCompareFunction compare>
void executeCompareTmpTmp(Executer &executer)
{
    auto rhs = executer.moveTopTmpStr();
    executer.pop();
    auto lhs = executer.moveTopTmpStr();
    executer.setTopIntFromBool(compare(lhs.get(), rhs.get()));
}

// ----------------------------------------

inline bool lt(double lhs, double rhs)
{
    return lhs < rhs;
}

inline bool lt(int32_t lhs, int32_t rhs)
{
    return lhs < rhs;
}

inline bool lt(const std::string *lhs, const std::string *rhs)
{
    return *lhs < *rhs;
}

OperatorCode<OpType::DblDbl> lt_dbl_dbl_code {recreateBinaryOperator, executeCompareDblDbl<lt>};
OperatorCode<OpType::IntDbl> lt_int_dbl_code {recreateBinaryOperator, executeCompareIntDbl<lt>};
OperatorCode<OpType::DblInt> lt_dbl_int_code {recreateBinaryOperator, executeCompareDblInt<lt>};
OperatorCode<OpType::IntInt> lt_int_int_code {recreateBinaryOperator, executeCompareIntInt<lt>};
OperatorCode<OpType::StrStr> lt_str_str_code {recreateBinaryOperator, executeCompareStrStr<lt>};
OperatorCode<OpType::TmpStr> lt_tmp_str_code {recreateBinaryOperator, executeCompareTmpStr<lt>};
OperatorCode<OpType::StrTmp> lt_str_tmp_code {recreateBinaryOperator, executeCompareStrTmp<lt>};
OperatorCode<OpType::TmpTmp> lt_tmp_tmp_code {recreateBinaryOperator, executeCompareTmpTmp<lt>};

CompOperatorCodes lt_codes {
    Precedence::Relation, "<",
    lt_dbl_dbl_code, lt_int_dbl_code, lt_dbl_int_code, lt_int_int_code,
    lt_str_str_code, lt_tmp_str_code, lt_str_tmp_code, lt_tmp_tmp_code
};

// ----------------------------------------

inline bool gt(double lhs, double rhs)
{
    return lhs > rhs;
}

inline bool gt(int32_t lhs, int32_t rhs)
{
    return lhs > rhs;
}

inline bool gt(const std::string *lhs, const std::string *rhs)
{
    return *lhs > *rhs;
}

OperatorCode<OpType::DblDbl> gt_dbl_dbl_code {recreateBinaryOperator, executeCompareDblDbl<gt>};
OperatorCode<OpType::IntDbl> gt_int_dbl_code {recreateBinaryOperator, executeCompareIntDbl<gt>};
OperatorCode<OpType::DblInt> gt_dbl_int_code {recreateBinaryOperator, executeCompareDblInt<gt>};
OperatorCode<OpType::IntInt> gt_int_int_code {recreateBinaryOperator, executeCompareIntInt<gt>};
OperatorCode<OpType::StrStr> gt_str_str_code {recreateBinaryOperator, executeCompareStrStr<gt>};
OperatorCode<OpType::TmpStr> gt_tmp_str_code {recreateBinaryOperator, nullptr};
OperatorCode<OpType::StrTmp> gt_str_tmp_code {recreateBinaryOperator, nullptr};
OperatorCode<OpType::TmpTmp> gt_tmp_tmp_code {recreateBinaryOperator, nullptr};

CompOperatorCodes gt_codes {
    Precedence::Relation, ">",
    gt_dbl_dbl_code, gt_int_dbl_code, gt_dbl_int_code, gt_int_int_code,
    gt_str_str_code, gt_tmp_str_code, gt_str_tmp_code, gt_tmp_tmp_code
};

// ----------------------------------------

inline bool le(double lhs, double rhs)
{
    return lhs <= rhs;
}

inline bool le(int32_t lhs, int32_t rhs)
{
    return lhs <= rhs;
}

inline bool le(const std::string *lhs, const std::string *rhs)
{
    return *lhs <= *rhs;
}

OperatorCode<OpType::DblDbl> le_dbl_dbl_code {recreateBinaryOperator, executeCompareDblDbl<le>};
OperatorCode<OpType::IntDbl> le_int_dbl_code {recreateBinaryOperator, executeCompareIntDbl<le>};
OperatorCode<OpType::DblInt> le_dbl_int_code {recreateBinaryOperator, executeCompareDblInt<le>};
OperatorCode<OpType::IntInt> le_int_int_code {recreateBinaryOperator, executeCompareIntInt<le>};
OperatorCode<OpType::StrStr> le_str_str_code {recreateBinaryOperator, executeCompareStrStr<le>};
OperatorCode<OpType::TmpStr> le_tmp_str_code {recreateBinaryOperator, nullptr};
OperatorCode<OpType::StrTmp> le_str_tmp_code {recreateBinaryOperator, nullptr};
OperatorCode<OpType::TmpTmp> le_tmp_tmp_code {recreateBinaryOperator, nullptr};

CompOperatorCodes le_codes {
    Precedence::Relation, "<=",
    le_dbl_dbl_code, le_int_dbl_code, le_dbl_int_code, le_int_int_code,
    le_str_str_code, le_tmp_str_code, le_str_tmp_code, le_tmp_tmp_code
};

// ----------------------------------------

inline bool ge(double lhs, double rhs)
{
    return lhs >= rhs;
}

inline bool ge(int32_t lhs, int32_t rhs)
{
    return lhs >= rhs;
}

inline bool ge(const std::string *lhs, const std::string *rhs)
{
    return *lhs >= *rhs;
}

OperatorCode<OpType::DblDbl> ge_dbl_dbl_code {recreateBinaryOperator, executeCompareDblDbl<ge>};
OperatorCode<OpType::IntDbl> ge_int_dbl_code {recreateBinaryOperator, executeCompareIntDbl<ge>};
OperatorCode<OpType::DblInt> ge_dbl_int_code {recreateBinaryOperator, executeCompareDblInt<ge>};
OperatorCode<OpType::IntInt> ge_int_int_code {recreateBinaryOperator, executeCompareIntInt<ge>};
OperatorCode<OpType::StrStr> ge_str_str_code {recreateBinaryOperator, executeCompareStrStr<ge>};
OperatorCode<OpType::TmpStr> ge_tmp_str_code {recreateBinaryOperator, nullptr};
OperatorCode<OpType::StrTmp> ge_str_tmp_code {recreateBinaryOperator, nullptr};
OperatorCode<OpType::TmpTmp> ge_tmp_tmp_code {recreateBinaryOperator, nullptr};

CompOperatorCodes ge_codes {
    Precedence::Relation, ">=",
    ge_dbl_dbl_code, ge_int_dbl_code, ge_dbl_int_code, ge_int_int_code,
    ge_str_str_code, ge_tmp_str_code, ge_str_tmp_code, ge_tmp_tmp_code
};

// ----------------------------------------

inline bool eq(double lhs, double rhs)
{
    return lhs == rhs;
}

inline bool eq(int32_t lhs, int32_t rhs)
{
    return lhs == rhs;
}

inline bool eq(const std::string *lhs, const std::string *rhs)
{
    return *lhs == *rhs;
}

OperatorCode<OpType::DblDbl> eq_dbl_dbl_code {recreateBinaryOperator, executeCompareDblDbl<eq>};
OperatorCode<OpType::IntDbl> eq_int_dbl_code {recreateBinaryOperator, executeCompareIntDbl<eq>};
OperatorCode<OpType::DblInt> eq_dbl_int_code {recreateBinaryOperator, executeCompareDblInt<eq>};
OperatorCode<OpType::IntInt> eq_int_int_code {recreateBinaryOperator, executeCompareIntInt<eq>};
OperatorCode<OpType::StrStr> eq_str_str_code {recreateBinaryOperator, executeCompareStrStr<eq>};
OperatorCode<OpType::TmpStr> eq_tmp_str_code {recreateBinaryOperator, nullptr};
OperatorCode<OpType::StrTmp> eq_str_tmp_code {recreateBinaryOperator, nullptr};
OperatorCode<OpType::TmpTmp> eq_tmp_tmp_code {recreateBinaryOperator, nullptr};

CompOperatorCodes eq_codes {
    Precedence::Equality, "=",
    eq_dbl_dbl_code, eq_int_dbl_code, eq_dbl_int_code, eq_int_int_code,
    eq_str_str_code, eq_tmp_str_code, eq_str_tmp_code, eq_tmp_tmp_code
};

// ----------------------------------------

inline bool ne(double lhs, double rhs)
{
    return lhs != rhs;
}

inline bool ne(int32_t lhs, int32_t rhs)
{
    return lhs != rhs;
}

inline bool ne(const std::string *lhs, const std::string *rhs)
{
    return *lhs != *rhs;
}

OperatorCode<OpType::DblDbl> ne_dbl_dbl_code {recreateBinaryOperator, executeCompareDblDbl<ne>};
OperatorCode<OpType::IntDbl> ne_int_dbl_code {recreateBinaryOperator, executeCompareIntDbl<ne>};
OperatorCode<OpType::DblInt> ne_dbl_int_code {recreateBinaryOperator, executeCompareDblInt<ne>};
OperatorCode<OpType::IntInt> ne_int_int_code {recreateBinaryOperator, executeCompareIntInt<ne>};
OperatorCode<OpType::StrStr> ne_str_str_code {recreateBinaryOperator, executeCompareStrStr<ne>};
OperatorCode<OpType::TmpStr> ne_tmp_str_code {recreateBinaryOperator, nullptr};
OperatorCode<OpType::StrTmp> ne_str_tmp_code {recreateBinaryOperator, nullptr};
OperatorCode<OpType::TmpTmp> ne_tmp_tmp_code {recreateBinaryOperator, nullptr};

CompOperatorCodes ne_codes {
    Precedence::Equality, "<>",
    ne_dbl_dbl_code, ne_int_dbl_code, ne_dbl_int_code, ne_int_int_code,
    ne_str_str_code, ne_tmp_str_code, ne_str_tmp_code, ne_tmp_tmp_code
};
