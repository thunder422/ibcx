/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_POWERINTINT_H
#define IBC_POWERINTINT_H

#include <cmath>
#include <limits>

#include "executer.h"
#include "runerror.h"


struct PowerIntInt {
    PowerIntInt(Executer &executer, int32_t x, int32_t y);
    int32_t operator()();

private:
    int32_t calculateNegativeExponent();
    int32_t calculatePower();
    int32_t caculateForPositiveValue();
    int32_t multiplyPositiveValue();
    int32_t useDoublePowerForPositiveValue();
    int32_t calculateForNegativeValue();
    int32_t multiplyNegativeValue();
    int32_t useDoublePowerForNegativeValue();
    template <typename T> void checkPostiveOverflow(T result);
    template <typename T> void checkAnyOverflow(T result);

    Executer &executer;
    int32_t x;
    int32_t y;
};

inline PowerIntInt::PowerIntInt(Executer &executer, int32_t x, int32_t y) :
    executer {executer},
    x {x},
    y {y}
{
}

inline int32_t PowerIntInt::operator()()
{
    if (y < 0) {
        if (x == 0) {
            throw RunError {"divide by zero", executer.currentOffset()};
        }
        return calculateNegativeExponent();
    }
    return calculatePower();
}

inline int32_t PowerIntInt::calculateNegativeExponent()
{
    if (x == 1) {
        return 1;
    } else if (x == -1) {
        return (y & 1) ? -1 : 1;
    } else {
        return 0;
    }
}

inline int32_t PowerIntInt::calculatePower()
{
    return x >= 0 ? caculateForPositiveValue() : calculateForNegativeValue();
}

inline int32_t PowerIntInt::caculateForPositiveValue()
{
    constexpr int32_t MaximumPowerExponent = 19;
    return y < MaximumPowerExponent ? multiplyPositiveValue() : useDoublePowerForPositiveValue();
}

inline int32_t PowerIntInt::multiplyPositiveValue()
{
    auto result = 1ll;
    for (int i = 0; i < y; ++i) {
        result *= x;
        checkPostiveOverflow(result);
    }
    return static_cast<int32_t>(result);
}

inline int32_t PowerIntInt::useDoublePowerForPositiveValue()
{
    auto result = std::pow(x, y);
    checkPostiveOverflow(result);
    return static_cast<int32_t>(result);
}

inline int32_t PowerIntInt::calculateForNegativeValue()
{
    constexpr int32_t MaximumPowerExponent = 17;
    return y < MaximumPowerExponent ? multiplyNegativeValue() : useDoublePowerForNegativeValue();
}

inline int32_t PowerIntInt::multiplyNegativeValue()
{
    auto result = 1ll;
    for (int i = 0; i < y; ++i) {
        result *= x;
        checkAnyOverflow(result);
    }
    return static_cast<int32_t>(result);
}

inline int32_t PowerIntInt::useDoublePowerForNegativeValue()
{
    auto result = std::pow(x, y);
    checkAnyOverflow(result);
    return static_cast<int32_t>(result);
}

template <typename T>
inline void PowerIntInt::checkPostiveOverflow(T result)
{
    if (result > std::numeric_limits<int32_t>::max()) {
        throw RunError {"overflow", executer.currentOffset()};
    }
}

template <typename T>
inline void PowerIntInt::checkAnyOverflow(T result)
{
    if (result > std::numeric_limits<int32_t>::max()
            || result < std::numeric_limits<int32_t>::min()) {
        throw RunError {"overflow", executer.currentOffset()};
    }
}


#endif  // IBC_POWERINTINT_H
