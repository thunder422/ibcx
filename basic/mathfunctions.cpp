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

FunctionCode<ArgType::Dbl> abs_dbl_code {recreateFunctionWithOneArgument, executeAbsDbl};
FunctionCode<ArgType::Int> abs_int_code {recreateFunctionWithOneArgument, executeAbsInt};

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

FunctionCode<ArgType::Dbl> sgn_dbl_code {recreateFunctionWithOneArgument, executeSgnDbl};
FunctionCode<ArgType::Int> sgn_int_code {recreateFunctionWithOneArgument, executeSgnInt};

MultiTypeFunctionCodes sgn_codes {"SGN", sgn_dbl_code, sgn_int_code};

// ----------------------------------------

void executeSqr(Executer &executer)
{
    auto argument = executer.topDbl();
    if (argument < 0) {
        throw RunError {"square root of negative number", executer.currentOffset()};
    }
    executer.setTop(std::sqrt(argument));
}

FunctionCode<ArgType::Dbl> sqr_code {recreateFunctionWithOneArgument, executeSqr};

MathFunctionCodes sqr_codes {"SQR", sqr_code};

// ----------------------------------------

void executeInt(Executer &executer)
{
    executer.setTop(std::floor(executer.topDbl()));
}

FunctionCode<ArgType::Dbl> int_code {recreateFunctionWithOneArgument, executeInt};

MathFunctionCodes int_codes {"INT", int_code};

// ----------------------------------------

void executeFix(Executer &executer)
{
    executer.setTop(std::trunc(executer.topDbl()));
}

FunctionCode<ArgType::Dbl> fix_code {recreateFunctionWithOneArgument, executeFix};

MathFunctionCodes fix_codes {"FIX", fix_code};

// ----------------------------------------

void executeFrac(Executer &executer)
{
    auto argument = executer.topDbl();
    executer.setTop(argument - std::trunc(argument));
}

FunctionCode<ArgType::Dbl> frac_code {recreateFunctionWithOneArgument, executeFrac};

MathFunctionCodes frac_codes {"FRAC", frac_code};

// ----------------------------------------

void executeCos(Executer &executer)
{
    executer.setTop(std::cos(executer.topDbl()));
}

FunctionCode<ArgType::Dbl> cos_code {recreateFunctionWithOneArgument, executeCos};

MathFunctionCodes cos_codes {"COS", cos_code};

// ----------------------------------------

void executeSin(Executer &executer)
{
    executer.setTop(std::sin(executer.topDbl()));
}

FunctionCode<ArgType::Dbl> sin_code {recreateFunctionWithOneArgument, executeSin};

MathFunctionCodes sin_codes {"SIN", sin_code};

// ----------------------------------------

void executeTan(Executer &executer)
{
    executer.setTop(std::tan(executer.topDbl()));
}

FunctionCode<ArgType::Dbl> tan_code {recreateFunctionWithOneArgument, executeTan};

MathFunctionCodes tan_codes {"TAN", tan_code};

// ----------------------------------------

void executeAtn(Executer &executer)
{
    executer.setTop(std::atan(executer.topDbl()));
}

FunctionCode<ArgType::Dbl> atn_code {recreateFunctionWithOneArgument, executeAtn};

MathFunctionCodes atn_codes {"ATN", atn_code};

// ----------------------------------------

void executeLog(Executer &executer)
{
    auto argument = executer.topDbl();
    if (argument <= 0) {
        throw RunError {"logarithm of non-positive number", executer.currentOffset()};
    }
    executer.setTop(std::log(argument));
}

FunctionCode<ArgType::Dbl> log_code {recreateFunctionWithOneArgument, executeLog};

MathFunctionCodes log_codes {"LOG", log_code};

// ----------------------------------------

void executeExp(Executer &executer)
{
    auto result = std::exp(executer.topDbl());
    checkForOverflow(executer, result);
    executer.setTop(result);
}

FunctionCode<ArgType::Dbl> exp_code {recreateFunctionWithOneArgument, executeExp};

static MathFunctionCodes exp_codes {"EXP", exp_code};

// ----------------------------------------

void executeCvtDbl(Executer &executer)
{
    executer.setTop(executer.topIntAsDbl());
}

Code cvtdbl_code {recreateNothing, executeCvtDbl};

FunctionCode<ArgType::Int> cdbl_code {recreateFunctionWithOneArgument, executeCvtDbl};

ConvertFunctionCodes cdbl_codes {"CDBL", cdbl_code};

// ----------------------------------------

void executeCvtInt(Executer &executer)
{
    auto operand = std::round(executer.topDbl());
    checkIntegerOverflow(executer, operand);
    executer.setTopIntFromDouble(operand);
}

void recreateCvtInt(Recreator &recreator)
{
    recreator.markOperandIfError();
}

Code cvtint_code {recreateCvtInt, executeCvtInt};

FunctionCode<ArgType::Dbl> cint_code {recreateFunctionWithOneArgument, executeCvtInt};

ConvertFunctionCodes cint_codes {"CINT", cint_code};

// ----------------------------------------

void executeRnd(Executer &executer)
{
    executer.push(executer.getRandomNumber());
}

void executeRndInt(Executer &executer)
{
    auto argument = executer.topInt();
    if (argument <= 0) {
        throw RunError {"random function on zero or negative number", executer.currentOffset()};
    }
    executer.setTop(executer.getRandomNumber(argument));
}

FunctionCode<ArgType::None> rnd_code {recreateFunctionWithNoArguments, executeRnd};
FunctionCode<ArgType::Int> rnd_int_code {recreateFunctionWithOneArgument, executeRndInt};

RandomFunctionCodes rnd_codes {"RND", rnd_code, rnd_int_code};
