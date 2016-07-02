/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_POWERDBLINT_H
#define IBC_POWERDBLINT_H

#include <cmath>

#include "runerror.h"


struct PowerDblInt {
    PowerDblInt(double x, int y);
    double operator()();

private:
    double multiplyForPositiveExponent();
    double divideForNegativeExponent();

    double x;
    int y;
};

inline PowerDblInt::PowerDblInt(double x, int y) :
    x {x},
    y {y}
{
}

inline double PowerDblInt::operator()()
{
    if (y > 0) {
        return multiplyForPositiveExponent();
    } else {
        return divideForNegativeExponent();
    }
}

inline double PowerDblInt::multiplyForPositiveExponent()
{
    double result = 1;
    while (--y >= 0) {
        result *= x;
    }
    if (result == HUGE_VAL) {
        throw RunError {"overflow", 0};
    }
    return result;
}

inline double PowerDblInt::divideForNegativeExponent()
{
    if (x == 0) {
        throw RunError {"divide by zero", 0};
    }
    double result = 1;
    while (++y <= 0) {
        result /= x;
    }
    return result;
}


#endif  // IBC_POWERDBLINT_H
