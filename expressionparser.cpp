/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>

#include "compiler.h"
#include "constnumparser.h"
#include "expressionparser.h"
#include "parseerror.h"
#include "programcode.h"
#include "programunit.h"


class ExpressionParser::Impl {
public:
    Impl(Compiler compiler);

    DataType parseExpression(DataType expected_data_type);

private:
    DataType parseNumOperand();

    Compiler compiler;
};

ExpressionParser::ExpressionParser(Compiler &compiler) :
    pimpl {new Impl(compiler)}
{
}

DataType ExpressionParser::operator ()(DataType expected_data_type)
{
    return pimpl->parseExpression(expected_data_type);
}

ExpressionParser::~ExpressionParser()
{
}

// ----------------------------------------

ExpressionParser::Impl::Impl(Compiler compiler) :
    compiler {compiler}
{
}

DataType ExpressionParser::Impl::parseExpression(DataType expected_data_type)
{
    (void)expected_data_type;
    return parseNumOperand();
}

DataType ExpressionParser::Impl::parseNumOperand()
{
    ConstNumParser parse_constant {compiler};
    unsigned column = compiler.is.tellg();
    auto data_type = parse_constant();
    if (data_type == DataType::Null) {
        throw ParseError {"expected numeric expression", column};
    }
    return data_type;
}
