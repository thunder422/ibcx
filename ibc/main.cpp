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


void usage();

int main(int argc, char *argv[])
{
    std::vector<std::string> args {argv, argv + argc};

    if (args.size() == 1) {
        usage();
        return 1;
    }
    if (args.size() > 3) {
        std::cerr << "ibc: extra operand '" << args[3] << "'" << std::endl;
        usage();
        return 1;
    }

    std::string file_name = args.size() == 2 ? args[1] : args[2];
    bool also_recreate;
    if (args.size() == 3) {
        if (args[1] != "-r") {
            std::cerr << "ibc: invalid option -- '" << args[1] << "'" << std::endl;
            usage();
            return 1;
        }
        also_recreate = true;
    } else {
        also_recreate = false;
    }

    std::ifstream ifs(file_name);
    if (!ifs.is_open()) {
        std::cerr << "ibc: " << args[1] << ": could not open file" << std::endl;
        return 1;
    }

    ProgramUnit program;

    if (!program.compileSource(ifs, std::cerr)) {
        std::cerr << args[1] << ": contains errros, program not run" << std::endl;
        return 1;
    }
    if (also_recreate) {
        std::cout << "Program:" << std::endl;
        program.recreate(std::cout);
        std::cout << std::endl << "Executing..." << std::endl;
    }
    program.run(std::cout);
}

void usage()
{
    std::cerr << "usage: ibc [-r] <source-file>" << std::endl;
}
