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


inline bool withinIntegerRange(double value)
{
    auto upper_out_of_range = static_cast<double>(std::numeric_limits<int32_t>::max()) + 0.5;
    auto lower_out_of_range = static_cast<double>(std::numeric_limits<int32_t>::min()) - 0.5;
    return value > lower_out_of_range && value < upper_out_of_range;
}

inline bool withinIntegerRange(int64_t value)
{
    return value >= std::numeric_limits<int32_t>::min()
        && value <= std::numeric_limits<int32_t>::max();
}

template <typename T>
inline void checkIntegerOverflow(Executer &executer, T result)
{
    if (!withinIntegerRange(result)) {
        throw RunError {"overflow", executer.currentOffset()};
    }
}

inline void checkNegativeIntegerOverflow(Executer &executer, int32_t value)
{
    if (value == std::numeric_limits<int32_t>::min()) {
        throw RunError {"overflow", executer.currentOffset()};
    }
}

inline void checkDoubleOverflow(Executer &executer, double result)
{
    if (std::fabs(result) > std::numeric_limits<double>::max()) {
        throw RunError {"overflow", executer.currentOffset()};
    }
}

inline void checkForOverflow(Executer &executer, double result)
{
    if (result == HUGE_VAL) {
        throw RunError {"overflow", executer.currentOffset()};
    }
}


#endif  // IBC_OVERFLOW_H
