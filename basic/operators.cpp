/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "code.h"
#include "executer.h"
#include "recreator.h"


void recreateNegate(Recreator &recreator);
void executeNegateDbl(Executer &executer);
void executeNegateInt(Executer &executer);

Code neg_dbl_code {recreateNegate, executeNegateDbl};
Code neg_int_code {recreateNegate, executeNegateInt};


void recreateNegate(Recreator &recreator)
{
    std::string string {"-"};
    recreator.swapTop(string);
    char c = string.front();
    if (isdigit(c) || c == '.') {
        recreator.append(' ');
    }
    recreator.append(string);
}

void executeNegateDbl(Executer &executer)
{
    executer.top().dbl_value = -executer.top().dbl_value;
}

void executeNegateInt(Executer &executer)
{
    executer.top().int_value = -executer.top().int_value;
}
