/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_OVERFLOW_H
#define IBC_OVERFLOW_H

#include <cmath>
#include <limits>

#include "executer.h"
#include "runerror.h"


template <typename T>
inline void checkIntegerOverflow(Executer &executer, T result)
{
    if (result > std::numeric_limits<int>::max() || result < std::numeric_limits<int>::min()) {
        throw RunError {"overflow", executer.currentOffset()};
    }
}

inline void checkDoubleOverflow(Executer &executer, double result)
{
    if (std::fabs(result) > std::numeric_limits<double>::max()) {
        throw RunError {"overflow", executer.currentOffset()};
    }
}


#endif  // IBC_OVERFLOW_H
