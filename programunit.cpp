/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <sstream>

#include "commandcompiler.h"
#include "compiler.h"
#include "executer.h"
#include "programreader.h"
#include "programunit.h"
#include "recreator.h"


ProgramUnit::ProgramUnit()
{
}

void ProgramUnit::compileSource(std::istream &is)
{
    std::string line;
    while (std::getline(is, line)) {
        auto code_line = CommandCompiler{line, *this}();
        appendCodeLine(code_line);
    }
}

void ProgramUnit::appendCodeLine(ProgramCode &code_line)
{
    line_info.emplace_back(code.size(), code_line.size());
    code.append(code_line);
}

std::string ProgramUnit::recreateLine(unsigned line_index)
{
    Recreator recreate(*this, createProgramReader(line_index));
    return recreate();
}

ProgramReader ProgramUnit::createProgramReader(unsigned line_index) const
{
    auto &info = line_info[line_index];
    return ProgramReader {code.begin(), info.offset, info.size};
}

Executer ProgramUnit::createExecutor(std::ostream &os) const
{
    return Executer {code.getBeginning(), const_num_dictionary.getDblValues(),
        const_num_dictionary.getIntValues(), os};
}
