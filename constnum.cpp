/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "code.h"
#include "recreator.h"


void const_num_recreate(Recreator &recreator)
{
    auto number = recreator.getConstNumOperand();
    recreator.push(number);
}

Code const_dbl_code {const_num_recreate};
Code const_int_code {const_num_recreate};
