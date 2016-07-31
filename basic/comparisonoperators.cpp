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
using IntCompareFunction = bool(*)(int, int);

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

// ----------------------------------------

bool lt(double lhs, double rhs)
{
    return lhs < rhs;
}

bool lt(int lhs, int rhs)
{
    return lhs < rhs;
}

OperatorCode<OpType::DblDbl> lt_dbl_dbl_code {recreateBinaryOperator, executeCompareDblDbl<lt>};
OperatorCode<OpType::IntDbl> lt_int_dbl_code {recreateBinaryOperator, executeCompareIntDbl<lt>};
OperatorCode<OpType::DblInt> lt_dbl_int_code {recreateBinaryOperator, executeCompareDblInt<lt>};
OperatorCode<OpType::IntInt> lt_int_int_code {recreateBinaryOperator, executeCompareIntInt<lt>};

NumOperatorCodes lt_codes {
    Precedence::Relation, "<",
    lt_dbl_dbl_code, lt_int_dbl_code, lt_dbl_int_code, lt_int_int_code
};

// ----------------------------------------

bool gt(double lhs, double rhs)
{
    return lhs > rhs;
}

bool gt(int lhs, int rhs)
{
    return lhs > rhs;
}

OperatorCode<OpType::DblDbl> gt_dbl_dbl_code {recreateBinaryOperator, executeCompareDblDbl<gt>};
OperatorCode<OpType::IntDbl> gt_int_dbl_code {recreateBinaryOperator, executeCompareIntDbl<gt>};
OperatorCode<OpType::DblInt> gt_dbl_int_code {recreateBinaryOperator, executeCompareDblInt<gt>};
OperatorCode<OpType::IntInt> gt_int_int_code {recreateBinaryOperator, executeCompareIntInt<gt>};

NumOperatorCodes gt_codes {
    Precedence::Relation, ">",
    gt_dbl_dbl_code, gt_int_dbl_code, gt_dbl_int_code, gt_int_int_code
};

// ----------------------------------------

bool le(double lhs, double rhs)
{
    return lhs <= rhs;
}

bool le(int lhs, int rhs)
{
    return lhs <= rhs;
}

OperatorCode<OpType::DblDbl> le_dbl_dbl_code {recreateBinaryOperator, executeCompareDblDbl<le>};
OperatorCode<OpType::IntDbl> le_int_dbl_code {recreateBinaryOperator, executeCompareIntDbl<le>};
OperatorCode<OpType::DblInt> le_dbl_int_code {recreateBinaryOperator, executeCompareDblInt<le>};
OperatorCode<OpType::IntInt> le_int_int_code {recreateBinaryOperator, executeCompareIntInt<le>};

NumOperatorCodes le_codes {
    Precedence::Relation, "<=",
    le_dbl_dbl_code, le_int_dbl_code, le_dbl_int_code, le_int_int_code
};

// ----------------------------------------

bool ge(double lhs, double rhs)
{
    return lhs >= rhs;
}

bool ge(int lhs, int rhs)
{
    return lhs >= rhs;
}

OperatorCode<OpType::DblDbl> ge_dbl_dbl_code {recreateBinaryOperator, executeCompareDblDbl<ge>};
OperatorCode<OpType::IntDbl> ge_int_dbl_code {recreateBinaryOperator, executeCompareIntDbl<ge>};
OperatorCode<OpType::DblInt> ge_dbl_int_code {recreateBinaryOperator, executeCompareDblInt<ge>};
OperatorCode<OpType::IntInt> ge_int_int_code {recreateBinaryOperator, executeCompareIntInt<ge>};

NumOperatorCodes ge_codes {
    Precedence::Relation, ">=",
    ge_dbl_dbl_code, ge_int_dbl_code, ge_dbl_int_code, ge_int_int_code
};
