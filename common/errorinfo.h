/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_ERRORINFO_H
#define IBC_ERRORINFO_H


constexpr char StartErrorMarker = '\02';

struct ErrorInfo {
    unsigned column;
    unsigned length;
};


#endif  // IBC_ERRORINFO_H
