/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_POWERDBLINT_H
#define IBC_POWERDBLINT_H


struct PowerDblInt {
    PowerDblInt(double x, int y);
    double operator()();

private:

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
    double result = 1;
    while (--y >= 0) {
        result *= x;
    }
    return result;
}


#endif  // IBC_POWERDBLINT_H
