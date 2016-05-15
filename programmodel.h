/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef PROGRAMMODEL_H
#define PROGRAMMODEL_H

#include "dictionary.h"


class ProgramModel {
public:
    ProgramModel();
    Dictionary &constDblDictionary();
    Dictionary &constIntDictionary();

private:
    Dictionary const_int_dictionary;
    Dictionary const_dbl_dictionary;
};


inline Dictionary &ProgramModel::constDblDictionary()
{
    return const_dbl_dictionary;
}

inline Dictionary &ProgramModel::constIntDictionary()
{
    return const_int_dictionary;
}


#endif  // PROGRAMMODEL_H
