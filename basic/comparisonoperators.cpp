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

OperatorCode<OpType::DblDbl> lt_dbl_dbl_code {recreateBinaryOperator, executeLtDblDbl};
OperatorCode<OpType::IntDbl> lt_int_dbl_code {recreateBinaryOperator, nullptr};
OperatorCode<OpType::DblInt> lt_dbl_int_code {recreateBinaryOperator, nullptr};
OperatorCode<OpType::IntInt> lt_int_int_code {recreateBinaryOperator, nullptr};

NumOperatorCodes lt_codes {
    Precedence::Relation, "<",
    lt_dbl_dbl_code, lt_int_dbl_code, lt_dbl_int_code, lt_int_int_code
};
