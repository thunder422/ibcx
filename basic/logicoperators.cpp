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
    executer.setTop(~executer.topInt());
}

OperatorCode<OpType::Int> not_code {recreateUnaryOperator, executeNot};

NotOperatorCodes not_codes {Precedence::Not, "NOT", not_code};

// ----------------------------------------

void executeAnd(Executer &executer)
{
    auto rhs = executer.topInt();
    executer.pop();
    executer.setTop(executer.topInt() & rhs);
}

OperatorCode<OpType::IntInt> and_code {recreateBinaryOperator, executeAnd};

LogicOperatorCodes and_codes {Precedence::And, "AND", and_code};

// ----------------------------------------

void executeXor(Executer &executer)
{
    auto rhs = executer.topInt();
    executer.pop();
    executer.setTop(executer.topInt() ^ rhs);
}

OperatorCode<OpType::IntInt> xor_code {recreateBinaryOperator, executeXor};

LogicOperatorCodes xor_codes {Precedence::Xor, "XOR", xor_code};

// ----------------------------------------

void executeOr(Executer &executer)
{
    auto rhs = executer.topInt();
    executer.pop();
    executer.setTop(executer.topInt() | rhs);
}

OperatorCode<OpType::IntInt> or_code {recreateBinaryOperator, executeOr};

LogicOperatorCodes or_codes {Precedence::Or, "OR", or_code};

// ----------------------------------------

void executeEqv(Executer &executer)
{
    auto rhs = executer.topInt();
    executer.pop();
    executer.setTop(~(executer.topInt() ^ rhs));
}

OperatorCode<OpType::IntInt> eqv_code {recreateBinaryOperator, executeEqv};

LogicOperatorCodes eqv_codes {Precedence::Eqv, "EQV", eqv_code};

// ----------------------------------------

void executeImp(Executer &executer)
{
    auto rhs = executer.topInt();
    executer.pop();
    executer.setTop(~executer.topInt() | rhs);
}

OperatorCode<OpType::IntInt> imp_code {recreateBinaryOperator, executeImp};

LogicOperatorCodes imp_codes {Precedence::Imp, "IMP", imp_code};
