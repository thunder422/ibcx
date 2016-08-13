/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_EXPRESSIONCOMPILER_H
#define IBC_EXPRESSIONCOMPILER_H

#include <iosfwd>
#include <memory>

#include <datatype.h>


class Compiler;

class ExpressionCompiler {
public:
    static std::unique_ptr<ExpressionCompiler> create(Compiler &compiler);

    virtual DataType compile(DataType expected_data_type) = 0;
    virtual ~ExpressionCompiler() = default;
};


#endif  // IBC_EXPRESSIONCOMPILER_H
