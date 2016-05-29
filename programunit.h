/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_PROGRAMMODEL_H
#define IBC_PROGRAMMODEL_H

#include <string>

#include "programcode.h"
#include "dictionary.h"


class ProgramReader;

class ProgramUnit {
public:
    ProgramUnit();

    void compileSource(std::istream &is);
    void appendCodeLine(ProgramCode &code_line);
    std::string recreateLine(unsigned line_index);
    ProgramReader createProgramReader(unsigned line_index) const;

    Dictionary &constNumDictionary();

private:
    struct LineInfo {
        LineInfo(unsigned offset, unsigned size);

        unsigned offset;
        unsigned size;
    };

    std::vector<LineInfo> line_info;
    ProgramCode code;
    Dictionary const_num_dictionary;
};


inline Dictionary &ProgramUnit::constNumDictionary()
{
    return const_num_dictionary;
}

inline ProgramUnit::LineInfo::LineInfo(unsigned offset, unsigned size) :
    offset {offset},
    size {size}
{
}


#endif  // IBC_PROGRAMMODEL_H
