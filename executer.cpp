/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "executer.h"

Executer::Executer()
{
}

void Executer::executeOneCode()
{
}

Executer::StackItem Executer::top()
{
    StackItem top;
    top.intValue = 12345;
    return top;
}
