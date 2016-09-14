/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "executer.h"
#include "operators.h"
#include "overflow.h"
#include "powerdblint.h"
#include "powerintint.h"
#include "recreator.h"


void executeNegateDbl(Executer &executer)
{
    executer.setTop(-executer.topDbl());
}

void executeNegateInt(Executer &executer)
{
    auto operand = executer.topInt();
    checkNegativeIntegerOverflow(executer, operand);
    executer.setTop(-operand);
}

OperatorCode<OpType::Dbl> neg_dbl_code {recreateUnaryOperator, executeNegateDbl};
OperatorCode<OpType::Int> neg_int_code {recreateUnaryOperator, executeNegateInt};

UnaryOperatorCodes neg_codes {Precedence::Negate, "-", neg_dbl_code, neg_int_code};

// ----------------------------------------

inline void validatePowerResult(double x, double result, Executer &executer);
inline void calculatePowerDblDbl(Executer &executer, double x, double y);

void executeExponentialDblDbl(Executer &executer)
{
    auto y = executer.topDbl();
    executer.pop();
    auto x = executer.topDbl();
    calculatePowerDblDbl(executer, x, y);
}

void executeExponentialIntDbl(Executer &executer)
{
    auto y = executer.topDbl();
    executer.pop();
    auto x = executer.topIntAsDbl();
    calculatePowerDblDbl(executer, x, y);
}

inline void calculatePowerDblDbl(Executer &executer, double x, double y)
{
    auto result = std::pow(x, y);
    validatePowerResult(x, result, executer);
    executer.setTop(result);
}

inline void validatePowerResult(double x, double result, Executer &executer)
{
    if (std::isnan(result)) {
        throw RunError {"domain error (non-integer exponent)", executer.currentOffset()};
    } else if (result == HUGE_VAL) {
        if (x == 0) {
            throw RunError {"divide by zero", executer.currentOffset()};
        } else {
            throw RunError {"overflow", executer.currentOffset()};
        }
    }
}

void executeExponentialDblInt(Executer &executer)
{
    auto y = executer.topInt();
    executer.pop();
    auto x = executer.topDbl();
    executer.setTop(PowerDblInt{executer, x, y}());
}

void executeExponentialIntInt(Executer &executer)
{
    auto y = executer.topInt();
    executer.pop();
    auto x = executer.topInt();
    executer.setTop(PowerIntInt{executer, x, y}());
}

OperatorCode<OpType::DblDbl> exp_dbl_dbl_code {recreateBinaryOperator, executeExponentialDblDbl};
OperatorCode<OpType::IntDbl> exp_int_dbl_code {recreateBinaryOperator, executeExponentialIntDbl};
OperatorCode<OpType::DblInt> exp_dbl_int_code {recreateBinaryOperator, executeExponentialDblInt};
OperatorCode<OpType::IntInt> exp_int_int_code {recreateBinaryOperator, executeExponentialIntInt};

NumOperatorCodes exp_codes {
    Precedence::Exponential, "^",
    exp_dbl_dbl_code, exp_int_dbl_code, exp_dbl_int_code, exp_int_int_code
};

// ----------------------------------------

inline void doDoubleMultiply(Executer &executer, double rhs);
inline void multiplyAndCheckResult(Executer &executer, double lhs, double rhs);

void executeMultiplyDblDbl(Executer &executer)
{
    auto rhs = executer.topDbl();
    doDoubleMultiply(executer, rhs);
}

void executeMultiplyIntDbl(Executer &executer)
{
    auto rhs = executer.topDbl();
    executer.pop();
    auto lhs = executer.topIntAsDbl();
    multiplyAndCheckResult(executer, lhs, rhs);
}

void executeMultiplyDblInt(Executer &executer)
{
    auto rhs = executer.topIntAsDbl();
    doDoubleMultiply(executer, rhs);
}

inline void doDoubleMultiply(Executer &executer, double rhs)
{
    executer.pop();
    auto lhs = executer.topDbl();
    multiplyAndCheckResult(executer, lhs, rhs);
}

inline void multiplyAndCheckResult(Executer &executer, double lhs, double rhs)
{
    auto result = lhs * rhs;
    checkDoubleOverflow(executer, result);
    executer.setTop(result);
}

void executeMultiplyIntInt(Executer &executer)
{
    auto rhs = executer.topInt();
    executer.pop();
    auto result = int64_t{executer.topInt()};
    result *= rhs;
    checkIntegerOverflow(executer, result);
    executer.setTopIntFromInt64(result);
}

OperatorCode<OpType::DblDbl> mul_dbl_dbl_code {recreateBinaryOperator, executeMultiplyDblDbl};
OperatorCode<OpType::IntDbl> mul_int_dbl_code {recreateBinaryOperator, executeMultiplyIntDbl};
OperatorCode<OpType::DblInt> mul_dbl_int_code {recreateBinaryOperator, executeMultiplyDblInt};
OperatorCode<OpType::IntInt> mul_int_int_code {recreateBinaryOperator, executeMultiplyIntInt};

NumOperatorCodes mul_codes {
    Precedence::Product, "*",
    mul_dbl_dbl_code, mul_int_dbl_code, mul_dbl_int_code, mul_int_int_code
};

// ----------------------------------------

template <typename T>
void checkDivideByZero(Executer &executer, T rhs)
{
    if (rhs == 0) {
        throw RunError {"divide by zero", executer.currentOffset()};
    }
}

inline double popDoubleDivisor(Executer &executer);
inline int32_t popIntegerDivisor(Executer &executer);
inline void divideAndCheckResult(Executer &executer, double lhs, double rhs);

void executeDivideDblDbl(Executer &executer)
{
    auto rhs = popDoubleDivisor(executer);
    auto lhs = executer.topDbl();
    divideAndCheckResult(executer, lhs, rhs);
}

void executeDivideIntDbl(Executer &executer)
{
    auto rhs = popDoubleDivisor(executer);
    auto lhs = executer.topIntAsDbl();
    divideAndCheckResult(executer, lhs, rhs);
}

inline double popDoubleDivisor(Executer &executer)
{
    auto rhs = executer.topDbl();
    checkDivideByZero(executer, rhs);
    executer.pop();
    return rhs;
}

void executeDivideDblInt(Executer &executer)
{
    auto rhs = static_cast<double>(popIntegerDivisor(executer));
    auto lhs = executer.topDbl();
    auto result = lhs / rhs;
    executer.setTop(result);
}

inline void divideAndCheckResult(Executer &executer, double lhs, double rhs)
{
    auto result = lhs / rhs;
    checkDoubleOverflow(executer, result);
    executer.setTop(result);
}

void executeDivideIntInt(Executer &executer)
{
    auto rhs = popIntegerDivisor(executer);
    executer.setTop(executer.topInt() / rhs);
}

inline int32_t popIntegerDivisor(Executer &executer)
{
    auto rhs = executer.topInt();
    checkDivideByZero(executer, rhs);
    executer.pop();
    return rhs;
}

OperatorCode<OpType::DblDbl> div_dbl_dbl_code {recreateBinaryOperator, executeDivideDblDbl};
OperatorCode<OpType::IntDbl> div_int_dbl_code {recreateBinaryOperator, executeDivideIntDbl};
OperatorCode<OpType::DblInt> div_dbl_int_code {recreateBinaryOperator, executeDivideDblInt};
OperatorCode<OpType::IntInt> div_int_int_code {recreateBinaryOperator, executeDivideIntInt};

NumOperatorCodes div_codes {
    Precedence::Product, "/",
    div_dbl_dbl_code, div_int_dbl_code, div_dbl_int_code, div_int_int_code
};

// ----------------------------------------

void executeIntegerDivide(Executer &executer)
{
    auto rhs = popDoubleDivisor(executer);
    auto lhs = executer.topDbl();
    auto result = lhs / rhs;
    checkIntegerOverflow(executer, result);
    executer.setTopIntFromDouble(result);
}

OperatorCode<OpType::DblDbl> int_div_code {recreateBinaryOperator, executeIntegerDivide};

IntDivOperatorCode int_div_codes {Precedence::IntDivide, "\\", int_div_code};

// ----------------------------------------

void executeModuloDblDbl(Executer &executer)
{
    auto rhs = popDoubleDivisor(executer);
    auto lhs = executer.topDbl();
    executer.setTop(std::fmod(lhs, rhs));
}

void executeModuloIntDbl(Executer &executer)
{
    auto rhs = popDoubleDivisor(executer);
    auto lhs = executer.topIntAsDbl();
    executer.setTop(std::fmod(lhs, rhs));
}

void executeModuloDblInt(Executer &executer)
{
    auto rhs = static_cast<double>(popIntegerDivisor(executer));
    auto lhs = executer.topDbl();
    executer.setTop(std::fmod(lhs, rhs));
}

void executeModuloIntInt(Executer &executer)
{
    auto rhs = popIntegerDivisor(executer);
    executer.setTop(executer.topInt() % rhs);
}

OperatorCode<OpType::DblDbl> mod_dbl_dbl_code {recreateBinaryOperator, executeModuloDblDbl};
OperatorCode<OpType::IntDbl> mod_int_dbl_code {recreateBinaryOperator, executeModuloIntDbl};
OperatorCode<OpType::DblInt> mod_dbl_int_code {recreateBinaryOperator, executeModuloDblInt};
OperatorCode<OpType::IntInt> mod_int_int_code {recreateBinaryOperator, executeModuloIntInt};

NumOperatorCodes mod_codes {
    Precedence::Modulo, "MOD",
    mod_dbl_dbl_code, mod_int_dbl_code, mod_dbl_int_code, mod_int_int_code
};

// ----------------------------------------

void executeAddDblDbl(Executer &executer)
{
    auto rhs = executer.topDbl();
    executer.pop();
    auto result = executer.topDbl() + rhs;
    checkDoubleOverflow(executer, result);
    executer.setTop(result);
}

void executeAddIntDbl(Executer &executer)
{
    auto rhs = executer.topDbl();
    executer.pop();
    executer.setTop(executer.topIntAsDbl() + rhs);
}

void executeAddDblInt(Executer &executer)
{
    auto rhs = executer.topIntAsDbl();
    executer.pop();
    executer.setTop(executer.topDbl() + rhs);
}

void executeAddIntInt(Executer &executer)
{
    auto rhs = executer.topInt();
    executer.pop();
    auto result = int64_t{executer.topInt()};
    result += rhs;
    checkIntegerOverflow(executer, result);
    executer.setTopIntFromInt64(result);
}

OperatorCode<OpType::DblDbl> add_dbl_dbl_code {recreateBinaryOperator, executeAddDblDbl};
OperatorCode<OpType::IntDbl> add_int_dbl_code {recreateBinaryOperator, executeAddIntDbl};
OperatorCode<OpType::DblInt> add_dbl_int_code {recreateBinaryOperator, executeAddDblInt};
OperatorCode<OpType::IntInt> add_int_int_code {recreateBinaryOperator, executeAddIntInt};
OperatorCode<OpType::StrStr> add_str_str_code {nullptr, nullptr};
OperatorCode<OpType::TmpStr> add_tmp_str_code {nullptr, nullptr};
OperatorCode<OpType::StrTmp> add_str_tmp_code {nullptr, nullptr};

NumStrOperatorCodes add_codes {
    Precedence::Summation, "+",
    add_dbl_dbl_code, add_int_dbl_code, add_dbl_int_code, add_int_int_code,
    add_str_str_code, add_tmp_str_code, add_str_tmp_code
};

// ----------------------------------------

void executeSubtractDblDbl(Executer &executer)
{
    auto rhs = executer.topDbl();
    executer.pop();
    auto result = executer.topDbl() - rhs;
    checkDoubleOverflow(executer, result);
    executer.setTop(result);
}

void executeSubtractIntDbl(Executer &executer)
{
    auto rhs = executer.topDbl();
    executer.pop();
    executer.setTop(executer.topIntAsDbl() - rhs);
}

void executeSubtractDblInt(Executer &executer)
{
    auto rhs = executer.topIntAsDbl();
    executer.pop();
    executer.setTop(executer.topDbl() - rhs);
}

void executeSubtractIntInt(Executer &executer)
{
    auto rhs = executer.topInt();
    executer.pop();
    auto result = int64_t{executer.topInt()};
    result -= rhs;
    checkIntegerOverflow(executer, result);
    executer.setTopIntFromInt64(result);
}

OperatorCode<OpType::DblDbl> sub_dbl_dbl_code {recreateBinaryOperator, executeSubtractDblDbl};
OperatorCode<OpType::IntDbl> sub_int_dbl_code {recreateBinaryOperator, executeSubtractIntDbl};
OperatorCode<OpType::DblInt> sub_dbl_int_code {recreateBinaryOperator, executeSubtractDblInt};
OperatorCode<OpType::IntInt> sub_int_int_code {recreateBinaryOperator, executeSubtractIntInt};

NumOperatorCodes sub_codes {
    Precedence::Summation, "-",
    sub_dbl_dbl_code, sub_int_dbl_code, sub_dbl_int_code, sub_int_int_code
};
