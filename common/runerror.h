/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_RUNERROR_H
#define IBC_RUNERROR_H

#include <stdexcept>


struct RunError : public std::runtime_error {
    RunError(const char *message, unsigned offset) : runtime_error {message}, offset {offset} { }

    unsigned offset;
};


#endif  // IBC_RUNERROR_H
