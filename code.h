/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef CODE_H
#define CODE_H

#include <cstdint>


class Code {
public:
    Code();

    uint16_t getValue();

private:
    static uint16_t count;

    uint16_t value;
};


inline uint16_t Code::getValue()
{
    return value;
}


#endif  // CODE_H
