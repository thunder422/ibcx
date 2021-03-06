/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_POWERDBLINT_H
#define IBC_POWERDBLINT_H

#include <cmath>

#include "executer.h"
#include "overflow.h"
#include "runerror.h"


struct PowerDblInt {
    PowerDblInt(Executer &executer, double x, int32_t y);
    double operator()();

private:
    double multiplyForPositiveExponent();
    double divideForNegativeExponent();
    double useDoublePower();

    Executer &executer;
    double x;
    int32_t y;
};

inline PowerDblInt::PowerDblInt(Executer &executer, double x, int32_t y) :
    executer {executer},
    x {x},
    y {y}
{
}

inline double PowerDblInt::operator()()
{
    if (y > 0) {
        constexpr int32_t MaximumPowerExponent = 19;
        return y < MaximumPowerExponent ? multiplyForPositiveExponent() : useDoublePower();
    } else {
        constexpr int32_t MinimumPowerExponent = -17;
        return y > MinimumPowerExponent ? divideForNegativeExponent() : useDoublePower();
    }
}

inline double PowerDblInt::multiplyForPositiveExponent()
{
    auto result = 1.0;
    while (--y >= 0) {
        result *= x;
    }
    checkForOverflow(executer, result);
    return result;
}

inline double PowerDblInt::divideForNegativeExponent()
{
    if (x == 0) {
        throw RunError {"divide by zero", executer.currentOffset()};
    }
    auto result = 1.0;
    while (++y <= 0) {
        result /= x;
    }
    return result;
}

inline double PowerDblInt::useDoublePower()
{
    auto result = std::pow(x, y);
    checkForOverflow(executer, result);
    return result;
}


#endif  // IBC_POWERDBLINT_H
