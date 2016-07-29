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
