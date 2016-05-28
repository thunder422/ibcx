/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_CONSTNUM_H
#define IBC_CONSTNUM_H

#include "code.h"


class ConstNumCode : public Code {
public:
    ConstNumCode();
    void recreate(Recreator &recreator) const override;
};


#endif  // IBC_CONSTNUM_H
