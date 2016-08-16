/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_CODES_H
#define IBC_CODES_H

#include <vector>

#include "datatype.h"
#include "wordtype.h"


class Code;

class Codes {
public:
    virtual std::vector<WordType> codeValues() const = 0;
};


#endif  // IBC_CODES_H
