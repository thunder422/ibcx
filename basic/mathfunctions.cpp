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
        executer.setTop(-argument);
    }
}

void executeAbsInt(Executer &executer)
{
    auto argument = executer.topInt();
    if (argument < 0) {
        checkNegativeIntegerOverflow(executer, argument);
        executer.setTop(-argument);
    }
}

FunctionCode<ArgType::Dbl> abs_dbl_code {recreateFunction, executeAbsDbl};
FunctionCode<ArgType::Int> abs_int_code {recreateFunction, executeAbsInt};

MultiTypeFunctionCodes abs_codes {"ABS", abs_dbl_code, abs_int_code};

// ----------------------------------------

template <typename T>
inline void executeSgn(Executer &executer)
{
    auto argument = executer.top<T>();
    if (argument > T{0}) {
        executer.setTop(T{1});
    } else if (argument < 0.0) {
        executer.setTop(T{-1});
    } else {
        executer.setTop(T{0});
    }
}

void executeSgnDbl(Executer &executer)
{
    executeSgn<double>(executer);
}

void executeSgnInt(Executer &executer)
{
    executeSgn<int32_t>(executer);
}

FunctionCode<ArgType::Dbl> sgn_dbl_code {recreateFunction, executeSgnDbl};
FunctionCode<ArgType::Int> sgn_int_code {recreateFunction, executeSgnInt};

MultiTypeFunctionCodes sgn_codes {"SGN", sgn_dbl_code, sgn_int_code};

// ----------------------------------------

FunctionCode<ArgType::Dbl> sqr_code {recreateFunction, nullptr};

MathFunctionCodes sqr_codes {"SQR", sqr_code};
