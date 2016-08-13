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
#include "programerror.h"
#include "programreader.h"
#include "programunit.h"
#include "recreator.h"
#include "runerror.h"


ProgramUnit::ProgramUnit()
{
}

bool ProgramUnit::compileSource(std::istream &is, std::ostream &os)
{
    auto program_errors = compile(is);
    for (auto &error : program_errors) {
        error.output(os);
    }
    return program_errors.empty();
}

std::vector<ProgramError> ProgramUnit::compile(std::istream &is)
{
    std::vector<ProgramError> errors;
    std::string line;
    while (std::getline(is, line)) {
        try {
            auto code_line = CommandCompiler::create(line, *this)->compile();
            appendCodeLine(code_line);
        }
        catch (const CompileError &error) {
            appendEmptyCodeLine();
            unsigned line_number = line_info.size();
            errors.emplace_back(error, line_number, line);
        }
    }
    return errors;
}

void ProgramUnit::appendEmptyCodeLine()
{
    ProgramCode empty_line;
    appendCodeLine(empty_line);
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

std::string ProgramUnit::recreateLine(unsigned line_index, unsigned error_offset) const
{
    auto recreator = Recreator::create(*this, createProgramReader(line_index), error_offset);
    return recreator->recreate();
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

bool ProgramUnit::runCode(std::ostream &os) noexcept
{
    try {
        run(os);
        return true;
    }
    catch (const ProgramError &error) {
        error.output(os);
        return false;
    }
}

void ProgramUnit::run(std::ostream &os)
{
    try {
        execute(os);
    }
    catch (const RunError &error) {
        generateProgramError(error);
    }
}

void ProgramUnit::generateProgramError(const RunError &error)
{
    if (error.offset >= code.size()) {
        throw ProgramError {error};
    } else {
        auto line_index = lineIndex(error.offset);
        auto program_line = recreateLine(line_index, error.offset);
        throw ProgramError {error, line_index + 1, program_line};
    }
}

void ProgramUnit::execute(std::ostream &os)
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
