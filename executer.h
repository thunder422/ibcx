/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_EXECUTER_H
#define IBC_EXECUTER_H

#include "wordtype.h"


class Executer {
public:
    union StackItem {
        int int_value;
    };

    Executer(const WordType *code, const int *const_int_values);
    void executeOneCode();

    StackItem top();

private:
    const WordType *code;
    const int *const_int_values;
};


#endif  // IBC_EXECUTER_H
