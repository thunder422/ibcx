/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <memory>


class Token {
public:
    Token();
    virtual ~Token();
};

using UniqueToken = std::unique_ptr<Token>;


#endif  // TOKEN_H
