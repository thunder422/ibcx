/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "executer.h"
#include "operators.h"
#include "recreator.h"


void executeNot(Executer &executer)
{
    executer.setTopInt(~executer.topInt());
}

OperatorCode<OpType::Int> not_code {recreateUnaryOperator, executeNot};

NotOperatorCodes not_codes {Precedence::Not, "NOT", not_code};

// ----------------------------------------

void executeAnd(Executer &executer)
{
    auto lhs = executer.topInt();
    executer.pop();
    executer.setTopInt(lhs & executer.topInt());
}

OperatorCode<OpType::IntInt> and_code {recreateBinaryOperator, executeAnd};

LogicOperatorCodes and_codes {Precedence::And, "AND", and_code};

// ----------------------------------------

void executeOr(Executer &executer)
{
    auto lhs = executer.topInt();
    executer.pop();
    executer.setTopInt(lhs | executer.topInt());
}

OperatorCode<OpType::IntInt> or_code {recreateBinaryOperator, executeOr};

LogicOperatorCodes or_codes {Precedence::Or, "OR", or_code};
