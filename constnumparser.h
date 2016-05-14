/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef CONSTANTNUMBERPARSER_H
#define CONSTANTNUMBERPARSER_H

#include "token.h"


class ConstNumParser {
public:
    ConstNumParser(std::istream &is_);

    UniqueToken getToken();

private:
    std::istream &is;
};


#endif  // CONSTANTNUMBERPARSER_H
