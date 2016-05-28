/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_PROGRAMMODEL_H
#define IBC_PROGRAMMODEL_H

#include "programcode.h"
#include "dictionary.h"


class ProgramReader;

class ProgramUnit {
public:
    ProgramUnit();
    Dictionary &constNumDictionary();
    void addCodeLine(ProgramCode &code_line);
    ProgramReader createProgramReader() const;

private:
    ProgramCode code;
    Dictionary const_num_dictionary;
};


inline Dictionary &ProgramUnit::constNumDictionary()
{
    return const_num_dictionary;
}


#endif  // IBC_PROGRAMMODEL_H
