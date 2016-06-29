/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "errorinfo.h"


ErrorInfo::ErrorInfo(const std::string &program_line) :
    column {program_line.find_first_of(StartErrorMarker)},
    length {1},
    line {program_line}
{
    line.erase(column, 1);
}
