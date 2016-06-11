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


struct CompileError;
class Executer;
class ProgramReader;

class ProgramUnit {
public:
    ProgramUnit();

    bool compileSource(std::istream &is, std::ostream &os);
    void appendCodeLine(ProgramCode &code_line);
    void recreate(std::ostream &os);
    std::string recreateLine(unsigned line_index);
    void run(std::ostream &os);
    Executer createExecutor(std::ostream &os) const;

    ConstNumDictionary &constNumDictionary();

private:
    void handleError(std::ostream &os, const std::string &line, const CompileError &error);
    void appendEmptyCodeLine();
    void outputError(std::ostream &os, const std::string &line, size_t line_number,
        const CompileError &error);
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