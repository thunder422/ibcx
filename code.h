/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_CODE_H
#define IBC_CODE_H

#include <cstdint>
#include <functional>
#include <vector>


class Recreator;

class Code {
public:
    static Code *getCode(uint16_t value);

    Code(std::function<void(Recreator &)> recreate_function);

    uint16_t getValue() const;
    void recreate(Recreator &recreator) const;

private:
    static uint16_t addCode(Code *code);
    static std::vector<Code *> codes;
    static std::vector<std::function<void(Recreator &)>> recreate_functions;

    uint16_t value;
};


inline uint16_t Code::getValue() const
{
    return value;
}


#endif  // IBC_CODE_H
