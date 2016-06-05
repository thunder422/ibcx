/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_CODE_H
#define IBC_CODE_H

#include <functional>
#include <vector>

#include "wordtype.h"


class Executer;
using ExecuteFunctionPointer = void(*)(Executer &);

class Recreator;

class Code {
public:
    static Code *getCode(WordType value);

    Code(std::function<void(Recreator &)> recreate_function,
        ExecuteFunctionPointer execute_function);

    WordType getValue() const;
    void recreate(Recreator &recreator) const;
    static const ExecuteFunctionPointer *getExecuteFunctions();

private:
    static WordType addCode(Code *code);
    static std::vector<Code *> codes;
    static std::vector<std::function<void(Recreator &)>> recreate_functions;
    static std::vector<ExecuteFunctionPointer> execute_functions;

    WordType value;
};


inline WordType Code::getValue() const
{
    return value;
}


#endif  // IBC_CODE_H
