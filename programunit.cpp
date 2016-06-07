/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <sstream>

#include "commandcode.h"
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

void ProgramUnit::recreate(std::ostream &os)
{
    for (unsigned line_index = 0; line_index < line_info.size(); ++line_index) {
        os << recreateLine(line_index) << std::endl;
    }
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

class ProgramEndGuard {
public:
    ProgramEndGuard(ProgramCode &code);
    ~ProgramEndGuard();

private:
    ProgramCode &code;
};

void ProgramUnit::run(std::ostream &os)
{
    ProgramEndGuard end_guard {code};
    auto executer = createExecutor(os);
    try {
        executer.run();
    }
    catch (const EndOfProgram &) {
    }
}

ProgramEndGuard::ProgramEndGuard(ProgramCode &code) :
    code {code}
{
    extern CommandCode end_code;
    code.emplace_back(end_code);
}

ProgramEndGuard::~ProgramEndGuard()
{
    code.pop_back();
}

Executer ProgramUnit::createExecutor(std::ostream &os) const
{
    return Executer {code.getBeginning(), const_num_dictionary.getDblValues(),
        const_num_dictionary.getIntValues(), os};
}
