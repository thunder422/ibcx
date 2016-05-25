/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef SUPPORT_H
#define SUPPORT_H


#define REQUIRE_OPERAND(expected_data_type, number) \
    REQUIRE(data_type == expected_data_type); \
    REQUIRE(code_line.size() == 2); \
    auto operand = code_line[1].operand(); \
    REQUIRE(program.constNumDictionary().get(operand) == number);


#endif  // SUPPORT_H
