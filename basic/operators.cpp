/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "code.h"
#include "recreator.h"


void negate_recreate(Recreator &recreator);

Code neg_dbl_code {nullptr, nullptr};
Code neg_int_code {negate_recreate, nullptr};


void negate_recreate(Recreator &recreator)
{
    std::string string {"-"};
    recreator.swapTop(string);
    if (isdigit(string.front())) {
        recreator.append(' ');
    }
    recreator.append(string);
}
