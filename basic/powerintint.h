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

#include "runerror.h"


struct PowerIntInt {
    PowerIntInt(int x, int y);
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

    int x;
    int y;
};

inline PowerIntInt::PowerIntInt(int x, int y) :
    x {x},
    y {y}
{
}

inline int PowerIntInt::operator()()
{
    if (y < 0) {
        if (x == 0) {
            throw RunError {"divide by zero", 0};
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
    return y < 19 ? multiplyPositiveValue() : useDoublePowerForPositiveValue();
}

inline int PowerIntInt::multiplyPositiveValue()
{
    int64_t result = 1;
    for (int i = 0; i < y; ++i) {
        result *= x;
        checkPostiveOverflow(result);
    }
    return result;
}

inline int PowerIntInt::useDoublePowerForPositiveValue()
{
    double result = std::pow(x, y);
    checkPostiveOverflow(result);
    return result;
}

inline int PowerIntInt::calculateForNegativeValue()
{
    return y < 17 ? multiplyNegativeValue() : useDoublePowerForNegativeValue();
}

inline int PowerIntInt::multiplyNegativeValue()
{
    int64_t result = 1;
    for (int i = 0; i < y; ++i) {
        result *= x;
        checkAnyOverflow(result);
    }
    return result;
}

inline int PowerIntInt::useDoublePowerForNegativeValue()
{
    double result = std::pow(x, y);
    checkAnyOverflow(result);
    return result;
}

template <typename T>
inline void PowerIntInt::checkPostiveOverflow(T result)
{
    if (result > std::numeric_limits<int>::max()) {
        throw RunError {"overflow", 0};
    }
}

template <typename T>
inline void PowerIntInt::checkAnyOverflow(T result)
{
    if (result > std::numeric_limits<int>::max() || result < std::numeric_limits<int>::min()) {
        throw RunError {"overflow", 0};
    }
}


#endif  // IBC_POWERINTINT_H
