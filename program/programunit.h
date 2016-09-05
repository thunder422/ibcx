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
#include "conststr.h"
#include "programcode.h"


struct CompileError;
struct ProgramError;
struct RunError;
class Executer;
class ProgramReader;

class ProgramUnit {
public:
    ProgramUnit();

    bool compileSource(std::istream &is, std::ostream &os);
    std::vector<ProgramError> compile(std::istream &is);
    void appendCodeLine(ProgramCode &code_line);
    void recreate(std::ostream &os);
    std::string recreateLine(unsigned line_index, unsigned error_offset = -1) const;
    bool runCode(std::ostream &os) noexcept;
    void run(std::ostream &os);
    Executer createExecutor(std::ostream &os) const;

    ConstNumDictionary &constNumDictionary();
    const ConstNumDictionary &constNumDictionary() const;
    ConstStrDictionary &constStrDictionary();

private:
    bool compileLine(const std::string &line);
    void appendEmptyCodeLine();
    ProgramReader createProgramReader(unsigned line_index) const;
    void generateProgramError(const RunError &error);
    void execute(std::ostream &os);
    unsigned lineIndex(unsigned offset) const;

    struct LineInfo {
        LineInfo(unsigned offset, unsigned size);

        unsigned offset;
        unsigned size;
    };

    std::vector<LineInfo> line_info;
    ProgramCode code;
    ConstNumDictionary const_num_dictionary;
    ConstStrDictionary const_str_dictionary;
};


inline ConstNumDictionary &ProgramUnit::constNumDictionary()
{
    return const_num_dictionary;
}

inline const ConstNumDictionary &ProgramUnit::constNumDictionary() const
{
    return const_num_dictionary;
}

inline ConstStrDictionary &ProgramUnit::constStrDictionary()
{
    return const_str_dictionary;
}

inline ProgramUnit::LineInfo::LineInfo(unsigned offset, unsigned size) :
    offset {offset},
    size {size}
{
}


#endif  // IBC_PROGRAMMODEL_H
