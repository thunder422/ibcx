/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef PROGRAMCODE_H
#define PROGRAMCODE_H

#include <vector>

#include "programword.h"


using ProgramVector = std::vector<ProgramWord>;


class ProgramCode {
public:
    ProgramCode();

private:
    ProgramVector code;
};


#endif  // PROGRAMCODE_H
