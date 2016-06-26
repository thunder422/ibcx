/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <algorithm>
#include <sstream>

#include "commandcode.h"
#include "commandcompiler.h"
#include "compileerror.h"
#include "compiler.h"
#include "executer.h"
#include "programreader.h"
#include "programunit.h"
#include "recreator.h"
#include "runerror.h"


ProgramUnit::ProgramUnit()
{
}

bool ProgramUnit::compileSource(std::istream &is, std::ostream &os)
{
    int error_count = 0;
    std::string line;
    while (std::getline(is, line)) {
        try {
            auto code_line = CommandCompiler{line, *this}();
            appendCodeLine(code_line);
        }
        catch (const CompileError &error) {
            ++error_count;
            handleError(os, line, error);
        }
    }
    return error_count == 0;
}

void ProgramUnit::handleError(std::ostream &os, const std::string &line, const CompileError &error)
{
    appendEmptyCodeLine();
    auto line_number = line_info.size();
    outputError(os, line, line_number, error);
}

void ProgramUnit::appendEmptyCodeLine()
{
    ProgramCode empty_line;
    appendCodeLine(empty_line);
}

void ProgramUnit::outputError(std::ostream &os, const std::string &line, size_t line_number,
    const CompileError &error)
{
    os << "error on line " << line_number << ':' << error.column << ": " << error.what()
        << std::endl;
    os << "    " << line << std::endl;
    std::string spaces(4 + error.column, ' ');
    std::string indicator(error.length, '^');
    os << spaces << indicator << std::endl;
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
        if (!executer.stackEmpty()) {
            throw RunError {"BUG: value stack not empty at end of program",
                executer.currentOffset()};
        }
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

unsigned ProgramUnit::lineIndex(unsigned offset) const
{
    auto find_offset = [offset](LineInfo line_info) {
        return offset >= line_info.offset && offset < line_info.offset + line_info.size;
    };

    auto it = std::find_if(line_info.begin(), line_info.end(), find_offset);
    return std::distance(line_info.cbegin(), it);
}
