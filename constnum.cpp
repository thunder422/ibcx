/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "constnum.h"
#include "recreator.h"


ConstNumCode::ConstNumCode()
{
}

void ConstNumCode::recreate(Recreator &recreator) const
{
    auto number = recreator.getConstNumOperand();
    recreator.push(number);
}

ConstNumCode const_dbl_code;
ConstNumCode const_int_code;
