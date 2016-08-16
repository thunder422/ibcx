/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_PRECEDENCE_H
#define IBC_PRECEDENCE_H

#include "cistring.h"
#include "wordtype.h"


enum class Precedence {
    Operand,
    Exponential,
    Negate,
    Product,
    IntDivide,
    Modulo,
    Summation,
    Relation,
    Equality,
    Not,
    And,
    Xor,
    Or,
    Eqv,
    Imp
};


class ComparisonOperator;
class FunctionCodes;
class OperatorCodes;

struct Table {
    static void addOperatorCodes(Precedence precedence, OperatorCodes &codes, const char *keyword);
    static void addNumFunctionCodes(FunctionCodes &codes, const char *keyword);
    static const char *getKeyword(WordType code_value);
    static Precedence getPrecedence(WordType code_value);
    static OperatorCodes *operatorCodes(Precedence precedence);
    static OperatorCodes *operatorCodes(Precedence precedence, char operator_char);
    static OperatorCodes *operatorCodes(Precedence precedence, const ci_string &word);
    static ComparisonOperator comparisonOperator(const std::string &keyword);
    static FunctionCodes *numFunctionCodes(const ci_string &word);
};


class ComparisonOperator {
public:
    ComparisonOperator();
    ComparisonOperator(OperatorCodes *codes, Precedence precedence);

    OperatorCodes *codes;
    Precedence precedence;
};

inline ComparisonOperator::ComparisonOperator() :
    codes {nullptr}
{
}

inline ComparisonOperator::ComparisonOperator(OperatorCodes *codes, Precedence precedence) :
    codes {codes},
    precedence {precedence}
{
}


#endif  // IBC_PRECEDENCE_H
