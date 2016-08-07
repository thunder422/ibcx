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


class ComparisonOperator;
class OperatorCodes;

struct Precedence {
    enum Level {
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
        Eqv
    };

    static void addOperatorCodes(Precedence::Level precedence, OperatorCodes &codes,
        const char *keyword);
    static const char *getKeyword(WordType code_value);
    static Precedence::Level getPrecedence(WordType code_value);
    static OperatorCodes *operatorCodes(Precedence::Level precedence);
    static OperatorCodes *operatorCodes(Precedence::Level precedence, char operator_char);
    static OperatorCodes *operatorCodes(Precedence::Level precedence, const ci_string &word);
    static ComparisonOperator comparisonOperator(const std::string &keyword);
};


class ComparisonOperator {
public:
    ComparisonOperator();
    ComparisonOperator(OperatorCodes *codes, Precedence::Level precedence);

    OperatorCodes *codes;
    Precedence::Level precedence;
};

inline ComparisonOperator::ComparisonOperator() :
    codes {nullptr}
{
}

inline ComparisonOperator::ComparisonOperator(OperatorCodes *codes,
        Precedence::Level precedence) :
    codes {codes},
    precedence {precedence}
{
}


#endif  // IBC_PRECEDENCE_H
