/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_CONSTANTNUMBERPARSER_H
#define IBC_CONSTANTNUMBERPARSER_H

#include <iosfwd>
#include <memory>

#include "datatype.h"


class Compiler;

class ConstNumCompiler {
public:
    ConstNumCompiler(Compiler &compiler);
    DataType operator()();
    bool negateOperator() const noexcept;
    char nextChar() const noexcept;
    ~ConstNumCompiler();

    class Impl;
private:
    std::unique_ptr<Impl> pimpl;
};


#endif  // IBC_CONSTANTNUMBERPARSER_H
