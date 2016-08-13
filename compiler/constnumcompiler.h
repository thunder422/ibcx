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
    static std::unique_ptr<ConstNumCompiler> create(Compiler &compiler);

    virtual DataType compile() = 0;
    virtual bool negateOperator() const noexcept = 0;
    virtual char unparsedChar() const noexcept = 0;
    virtual ~ConstNumCompiler() = default;
};


#endif  // IBC_CONSTANTNUMBERPARSER_H
