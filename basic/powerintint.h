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
    PowerIntInt(Executer &executer, int x, int y);
    int operator()();

private:
    int calculateNegativeExponent();
    int calculatePower();
    int caculateForPositiveValue();
    int multiplyPositiveValue();
    int useDoublePowerForPositiveValue();
    int calculateForNegativeValue();
    int multiplyNegativeValue();
    int useDoublePowerForNegativeValue();
    template <typename T> void checkPostiveOverflow(T result);
    template <typename T> void checkAnyOverflow(T result);

    Executer &executer;
    int x;
    int y;
};

inline PowerIntInt::PowerIntInt(Executer &executer, int x, int y) :
    executer {executer},
    x {x},
    y {y}
{
}

inline int PowerIntInt::operator()()
{
    if (y < 0) {
        if (x == 0) {
            throw RunError {"divide by zero", executer.currentOffset()};
        }
        return calculateNegativeExponent();
    }
    return calculatePower();
}

inline int PowerIntInt::calculateNegativeExponent()
{
    if (x == 1) {
        return 1;
    } else if (x == -1) {
        return (y & 1) ? -1 : 1;
    } else {
        return 0;
    }
}

inline int PowerIntInt::calculatePower()
{
    return x >= 0 ? caculateForPositiveValue() : calculateForNegativeValue();
}

inline int PowerIntInt::caculateForPositiveValue()
{
    constexpr int MaximumPowerExponent = 19;
    return y < MaximumPowerExponent ? multiplyPositiveValue() : useDoublePowerForPositiveValue();
}

inline int PowerIntInt::multiplyPositiveValue()
{
    auto result = 1ll;
    for (int i = 0; i < y; ++i) {
        result *= x;
        checkPostiveOverflow(result);
    }
    return static_cast<int>(result);
}

inline int PowerIntInt::useDoublePowerForPositiveValue()
{
    auto result = std::pow(x, y);
    checkPostiveOverflow(result);
    return static_cast<int>(result);
}

inline int PowerIntInt::calculateForNegativeValue()
{
    constexpr int MaximumPowerExponent = 17;
    return y < MaximumPowerExponent ? multiplyNegativeValue() : useDoublePowerForNegativeValue();
}

inline int PowerIntInt::multiplyNegativeValue()
{
    auto result = 1ll;
    for (int i = 0; i < y; ++i) {
        result *= x;
        checkAnyOverflow(result);
    }
    return static_cast<int>(result);
}

inline int PowerIntInt::useDoublePowerForNegativeValue()
{
    auto result = std::pow(x, y);
    checkAnyOverflow(result);
    return static_cast<int>(result);
}

template <typename T>
inline void PowerIntInt::checkPostiveOverflow(T result)
{
    if (result > std::numeric_limits<int>::max()) {
        throw RunError {"overflow", executer.currentOffset()};
    }
}

template <typename T>
inline void PowerIntInt::checkAnyOverflow(T result)
{
    if (result > std::numeric_limits<int>::max() || result < std::numeric_limits<int>::min()) {
        throw RunError {"overflow", executer.currentOffset()};
    }
}


#endif  // IBC_POWERINTINT_H
