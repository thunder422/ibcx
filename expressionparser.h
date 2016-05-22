/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef _IBC_EXPRESSIONPARSER_H
#define _IBC_EXPRESSIONPARSER_H

#include <iosfwd>
#include <memory>

#include <datatype.h>


class ProgramCode;
class ProgramUnit;

class ExpressionParser {
public:
    ExpressionParser(std::istream &is, ProgramCode &code_line, ProgramUnit &program);
    DataType operator()(DataType expected_data_type);
    ~ExpressionParser();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};


#endif  // _IBC_EXPRESSIONPARSER_H
