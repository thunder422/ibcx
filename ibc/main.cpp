/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "programunit.h"


struct IbcError { };


class IbcArguments {
public:
    IbcArguments(int argc, char *argv[]);
    const std::string &getFileName() const;
    bool getAlsoRecreate() const;

private:
    void checkNoArguments() const;
    void checkExtraArguments() const;
    void parseFileName();
    bool parseAlsoRecreate();
    void checkAlsoRecreate();
    void error(const char *message, const std::string &argument) const;
    void usage() const;

    std::vector<std::string> args;
    std::string file_name;
    bool also_recreate;
};


class IbcProgram {
public:
    IbcProgram(const IbcArguments &arguments);
    void compile();
    void recreate();
    void execute();

private:
    void checkFileOpen();

    std::string file_name;
    bool also_recreate;
    std::ifstream ifs;
    ProgramUnit program;
};


int main(int argc, char *argv[])
try
{
    IbcArguments arguments {argc, argv};

    IbcProgram program {arguments};

    program.compile();
    program.recreate();
    program.execute();
}
catch (const IbcError &) {
    return 1;
}

// ----------------------------------------

IbcArguments::IbcArguments(int argc, char *argv[]) :
    args {argv, argv + argc}
{
    checkNoArguments();
    checkExtraArguments();
    parseFileName();
    also_recreate = parseAlsoRecreate();
}

const std::string &IbcArguments::getFileName() const
{
    return file_name;
}

bool IbcArguments::getAlsoRecreate() const
{
    return also_recreate;
}

void IbcArguments::checkNoArguments() const
{
    if (args.size() == 1) {
        usage();
        throw IbcError {};
    }
}

void IbcArguments::checkExtraArguments() const
{
    if (args.size() > 3) {
        error("extra operand", args[3]);
        usage();
        throw IbcError {};
    }
}

void IbcArguments::parseFileName()
{
    file_name = args.size() == 2 ? args[1] : args[2];
}

bool IbcArguments::parseAlsoRecreate()
{
    if (args.size() == 3) {
        checkAlsoRecreate();
        return true;
    } else {
        return false;
    }
}

void IbcArguments::checkAlsoRecreate()
{
    if (args[1] != "-r") {
        error("invalid option --", args[1]);
        usage();
        throw IbcError {};
    }
}

void IbcArguments::error(const char *message, const std::string &argument) const
{
    std::cerr << "ibc: " << message << " '" << argument << "'" << std::endl;
}

void IbcArguments::usage() const
{
    std::cerr << "usage: ibc [-r] <source-file>" << std::endl;
}

// ----------------------------------------

IbcProgram::IbcProgram(const IbcArguments &arguments) :
    file_name {arguments.getFileName()},
    also_recreate {arguments.getAlsoRecreate()},
    ifs {file_name}
{
    checkFileOpen();
}

void IbcProgram::checkFileOpen()
{
    if (!ifs.is_open()) {
        std::cerr << "ibc: " << file_name << ": could not open file" << std::endl;
        throw IbcError {};
    }
}

void IbcProgram::compile()
{
    if (!program.compileSource(ifs, std::cerr)) {
        std::cerr << file_name << ": contains errros, program not run" << std::endl;
        throw IbcError {};
    }
}

void IbcProgram::recreate()
{
    if (also_recreate) {
        std::cout << "Program:" << std::endl;
        program.recreate(std::cout);
        std::cout << std::endl << "Executing..." << std::endl;
    }
}

void IbcProgram::execute()
{
    program.run(std::cout);
}
