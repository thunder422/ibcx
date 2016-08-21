/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "executer.h"
#include "functions.h"
#include "overflow.h"
#include "recreator.h"


void executeAbsDbl(Executer &executer)
{
    auto argument = executer.topDbl();
    if (argument < 0) {
        executer.setTopDbl(-argument);
    }
}

void executeAbsInt(Executer &executer)
{
    auto argument = executer.topInt();
    if (argument < 0) {
        checkNegativeIntegerOverflow(executer, argument);
        executer.setTopInt(-argument);
    }
}

FunctionCode<ArgType::Dbl> abs_dbl_code {recreateFunction, executeAbsDbl};
FunctionCode<ArgType::Int> abs_int_code {recreateFunction, executeAbsInt};

MultiTypeFunctionCodes abs_codes {"ABS", abs_dbl_code, abs_int_code};
