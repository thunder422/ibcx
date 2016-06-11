/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <fstream>
#include <iostream>

#include "programunit.h"

int main(int argc, char *argv[])
{
    if (argc == 1) {
        std::cerr << "usage: ibcx [-r] <source-file>" << std::endl;
        return 1;
    }

    std::ifstream ifs(argv[1]);
    if (!ifs.is_open()) {
        std::cerr << "ibc: " << argv[1] << ": could not open file" << std::endl;
        return 1;
    }

    ProgramUnit program;

    program.compileSource(ifs, std::cerr);
    program.run(std::cout);
}
