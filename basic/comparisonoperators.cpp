/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "executer.h"
#include "operators.h"
#include "recreator.h"


void executeLtDblDbl(Executer &executer)
{
    auto rhs = executer.topDbl();
    executer.pop();
    executer.setTopIntFromBool(executer.topDbl() < rhs);
}

void executeLtIntDbl(Executer &executer)
{
    auto rhs = executer.topDbl();
    executer.pop();
    executer.setTopIntFromBool(executer.topIntAsDbl() < rhs);
}

void executeLtDblInt(Executer &executer)
{
    auto rhs = executer.topIntAsDbl();
    executer.pop();
    executer.setTopIntFromBool(executer.topDbl() < rhs);
}

void executeLtIntInt(Executer &executer)
{
    auto rhs = executer.topInt();
    executer.pop();
    executer.setTopIntFromBool(executer.topInt() < rhs);
}

OperatorCode<OpType::DblDbl> lt_dbl_dbl_code {recreateBinaryOperator, executeLtDblDbl};
OperatorCode<OpType::IntDbl> lt_int_dbl_code {recreateBinaryOperator, executeLtIntDbl};
OperatorCode<OpType::DblInt> lt_dbl_int_code {recreateBinaryOperator, executeLtDblInt};
OperatorCode<OpType::IntInt> lt_int_int_code {recreateBinaryOperator, executeLtIntInt};

NumOperatorCodes lt_codes {
    Precedence::Relation, "<",
    lt_dbl_dbl_code, lt_int_dbl_code, lt_dbl_int_code, lt_int_int_code
};

// ----------------------------------------

void executeGtDblDbl(Executer &executer)
{
    auto rhs = executer.topDbl();
    executer.pop();
    executer.setTopIntFromBool(executer.topDbl() > rhs);
}

void executeGtIntDbl(Executer &executer)
{
    auto rhs = executer.topDbl();
    executer.pop();
    executer.setTopIntFromBool(executer.topIntAsDbl() > rhs);
}

void executeGtDblInt(Executer &executer)
{
    auto rhs = executer.topIntAsDbl();
    executer.pop();
    executer.setTopIntFromBool(executer.topDbl() > rhs);
}

void executeGtIntInt(Executer &executer)
{
    auto rhs = executer.topInt();
    executer.pop();
    executer.setTopIntFromBool(executer.topInt() > rhs);
}

OperatorCode<OpType::DblDbl> gt_dbl_dbl_code {recreateBinaryOperator, executeGtDblDbl};
OperatorCode<OpType::IntDbl> gt_int_dbl_code {recreateBinaryOperator, executeGtIntDbl};
OperatorCode<OpType::DblInt> gt_dbl_int_code {recreateBinaryOperator, executeGtDblInt};
OperatorCode<OpType::IntInt> gt_int_int_code {recreateBinaryOperator, executeGtIntInt};

NumOperatorCodes gt_codes {
    Precedence::Relation, ">",
    gt_dbl_dbl_code, gt_int_dbl_code, gt_dbl_int_code, gt_int_int_code
};

// ----------------------------------------

void executeLeDblDbl(Executer &executer)
{
    auto rhs = executer.topDbl();
    executer.pop();
    executer.setTopIntFromBool(executer.topDbl() <= rhs);
}

void executeLeIntDbl(Executer &executer)
{
    auto rhs = executer.topDbl();
    executer.pop();
    executer.setTopIntFromBool(executer.topIntAsDbl() <= rhs);
}

void executeLeDblInt(Executer &executer)
{
    auto rhs = executer.topIntAsDbl();
    executer.pop();
    executer.setTopIntFromBool(executer.topDbl() <= rhs);
}

void executeLeIntInt(Executer &executer)
{
    auto rhs = executer.topInt();
    executer.pop();
    executer.setTopIntFromBool(executer.topInt() <= rhs);
}

OperatorCode<OpType::DblDbl> le_dbl_dbl_code {recreateBinaryOperator, executeLeDblDbl};
OperatorCode<OpType::IntDbl> le_int_dbl_code {recreateBinaryOperator, executeLeIntDbl};
OperatorCode<OpType::DblInt> le_dbl_int_code {recreateBinaryOperator, executeLeDblInt};
OperatorCode<OpType::IntInt> le_int_int_code {recreateBinaryOperator, executeLeIntInt};

NumOperatorCodes le_codes {
    Precedence::Relation, "<=",
    le_dbl_dbl_code, le_int_dbl_code, le_dbl_int_code, le_int_int_code
};

// ----------------------------------------

OperatorCode<OpType::DblDbl> ge_dbl_dbl_code {nullptr, nullptr};
OperatorCode<OpType::IntDbl> ge_int_dbl_code {nullptr, nullptr};
OperatorCode<OpType::DblInt> ge_dbl_int_code {nullptr, nullptr};
OperatorCode<OpType::IntInt> ge_int_int_code {nullptr, nullptr};

NumOperatorCodes ge_codes {
    Precedence::Relation, ">=",
    ge_dbl_dbl_code, ge_int_dbl_code, ge_dbl_int_code, ge_int_int_code
};
