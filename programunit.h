/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_PROGRAMMODEL_H
#define IBC_PROGRAMMODEL_H

#include <string>

#include "constnum.h"
#include "programcode.h"


class Executer;
class ProgramReader;

class ProgramUnit {
public:
    ProgramUnit();

    void compileSource(std::istream &is);
    void appendCodeLine(ProgramCode &code_line);
    std::string recreateLine(unsigned line_index);
    Executer createExecutor() const;

    ConstNumDictionary &constNumDictionary();

private:
    ProgramReader createProgramReader(unsigned line_index) const;

    struct LineInfo {
        LineInfo(unsigned offset, unsigned size);

        unsigned offset;
        unsigned size;
    };

    std::vector<LineInfo> line_info;
    ProgramCode code;
    ConstNumDictionary const_num_dictionary;
};


inline ConstNumDictionary &ProgramUnit::constNumDictionary()
{
    return const_num_dictionary;
}

inline ProgramUnit::LineInfo::LineInfo(unsigned offset, unsigned size) :
    offset {offset},
    size {size}
{
}


#endif  // IBC_PROGRAMMODEL_H
