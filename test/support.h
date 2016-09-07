/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef SUPPORT_H
#define SUPPORT_H


#define REQUIRE_CODESIZE_OPERAND(expected_code_size, number) \
    auto code_line = compiler.getCodeLine(); \
    REQUIRE(code_line.size() == expected_code_size); \
    auto operand = code_line[1].operand(); \
    REQUIRE(program.getConstantNumber(operand) == number);

#define REQUIRE_DOUBLE_OPERAND(number) \
    REQUIRE(data_type.isDouble()); \
    REQUIRE_CODESIZE_OPERAND(2, number)

#define REQUIRE_INTEGER_OPERAND(number) \
    REQUIRE(data_type.isInteger()); \
    REQUIRE_CODESIZE_OPERAND(2, number)


#endif  // SUPPORT_H
