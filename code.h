/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_CODE_H
#define IBC_CODE_H

#include <cstdint>
#include <vector>


class Recreator;

class Code {
public:
    static Code *getCode(uint16_t value);

    Code();
    explicit Code(uint16_t value);

    uint16_t getValue() const;
    virtual void recreate(Recreator &recreator) const;

private:
    static uint16_t addCode(Code *code);
    static std::vector<Code *> codes;

    uint16_t value;
};


inline uint16_t Code::getValue() const
{
    return value;
}


#endif  // IBC_CODE_H
