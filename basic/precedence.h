/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_PRECEDENCE_H
#define IBC_PRECEDENCE_H

#include "wordtype.h"


struct Precedence {
    static const char *getKeyword(WordType code_value);
    static unsigned getPrecedence(WordType code_value);
};


#endif  // IBC_PRECEDENCE_H
