/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_PRECEDENCE_H
#define IBC_PRECEDENCE_H

#include "wordtype.h"


class OperatorCodes;

struct Precedence {
    enum Level {
        Operand,
        Exponential,
        Negate,
        Product
    };

    static const char *getKeyword(WordType code_value);
    static Precedence::Level getPrecedence(WordType code_value);
    static OperatorCodes *operatorCodes(Precedence::Level precedence);
    static OperatorCodes *operatorCodes(Precedence::Level precedence, char operator_char);
};


#endif  // IBC_PRECEDENCE_H
