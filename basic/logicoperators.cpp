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

OperatorCode<OpType::IntInt> and_code {recreateBinaryOperator, nullptr};

LogicOperatorCodes and_codes {Precedence::And, "AND", and_code};
