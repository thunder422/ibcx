/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "constnumparser.h"

ConstNumParser::ConstNumParser(std::istream &is_) :
    is(is_)
{
}

UniqueToken ConstNumParser::getToken()
{
    return UniqueToken();
}
