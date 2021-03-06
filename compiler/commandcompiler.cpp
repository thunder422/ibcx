/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>

#include "commandcode.h"
#include "commandcompiler.h"
#include "compiler.h"
#include "compileerror.h"
#include "programcode.h"


class CommandCompilerImpl : public CommandCompiler {
public:
    CommandCompilerImpl(const std::string &source_line, ProgramUnit &program);

    ProgramCode &&compile() override;

private:
    Compiler compiler;
};

// ----------------------------------------

std::unique_ptr<CommandCompiler> CommandCompiler::create(const std::string &source_line,
    ProgramUnit &program)
{
    return std::unique_ptr<CommandCompiler> {new CommandCompilerImpl {source_line, program}};
}

// ----------------------------------------

CommandCompilerImpl::CommandCompilerImpl(const std::string &source_line, ProgramUnit &program) :
    compiler {source_line, program}
{
}

ProgramCode &&CommandCompilerImpl::compile()
{
    if (compiler.peekNextChar() != EOF) {
        auto keyword = compiler.getKeyword();
        if (keyword.empty()) {
            throw CompileError {"expected command keyword", compiler.getColumn()};
        }
        auto code = CommandCode::find(keyword);
        compiler.clearWord();
        code->compile(compiler);
    }
    return compiler.getCodeLine();
}
