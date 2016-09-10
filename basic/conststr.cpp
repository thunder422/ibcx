/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "code.h"
#include "conststr.h"
#include "recreator.h"


WordType ConstStrDictionary::add(const std::string &string)
{
    auto entry = Dictionary::add(string);
    if (!entry.exists) {
        str_values.emplace_back(new std::string(string));
    }
    return entry.operand;
}


void recreateConstStr(Recreator &recreator)
{
    auto string = std::string {'"'};
    for (auto c : recreator.getConstStrOperand()) {
        string += c;
        if (c == '"') {
            string += c;
        }
    }
    string += '"';
    recreator.push(string);
}

Code const_str_code {recreateConstStr, nullptr};
