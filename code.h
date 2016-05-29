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
    using type = uint16_t;

    static Code *getCode(type value);

    Code(std::function<void(Recreator &)> recreate_function);

    type getValue() const;
    void recreate(Recreator &recreator) const;

private:
    static type addCode(Code *code);
    static std::vector<Code *> codes;
    static std::vector<std::function<void(Recreator &)>> recreate_functions;

    type value;
};


inline Code::type Code::getValue() const
{
    return value;
}


#endif  // IBC_CODE_H
