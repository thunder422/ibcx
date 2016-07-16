/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "code.h"
#include "executer.h"
#include "operators.h"
#include "overflow.h"
#include "powerdblint.h"
#include "powerintint.h"
#include "recreator.h"


UnaryOperatorCodes::UnaryOperatorCodes(OperatorCode<OpType::Dbl> &dbl_code,
        OperatorCode<OpType::Int> &int_code) :
    dbl_code {dbl_code},
    int_code {int_code}
{
}

OperatorInfo UnaryOperatorCodes::select(DataType data_type, DataType unused_data_type) const
{
    (void)unused_data_type;
    if (data_type == DataType::Double) {
        return OperatorInfo {dbl_code, DataType::Double};
    } else {
        return OperatorInfo {int_code, DataType::Integer};
    }
}

std::vector<WordType> UnaryOperatorCodes::codeValues() const
{
    return std::vector<WordType> {dbl_code.getValue(), int_code.getValue()};
}


NumOperatorCodes::NumOperatorCodes(OperatorCode<OpType::DblDbl> &dbl_dbl_code,
        OperatorCode<OpType::IntDbl> &int_dbl_code, OperatorCode<OpType::DblInt> &dbl_int_code,
        OperatorCode<OpType::IntInt> &int_int_code) :
    dbl_dbl_code {dbl_dbl_code},
    int_dbl_code {int_dbl_code},
    dbl_int_code {dbl_int_code},
    int_int_code {int_int_code}
{
}

OperatorInfo NumOperatorCodes::select(DataType lhs_data_type, DataType rhs_data_type) const
{
    if (lhs_data_type == DataType::Integer && rhs_data_type == DataType::Integer) {
        return OperatorInfo {int_int_code, DataType::Integer};
    } else if (lhs_data_type == DataType::Integer) {
        return OperatorInfo {int_dbl_code, DataType::Double};
    } else if (rhs_data_type == DataType::Integer) {
        return OperatorInfo {dbl_int_code, DataType::Double};
    } else {
        return OperatorInfo {dbl_dbl_code, DataType::Double};
    }
}

std::vector<WordType> NumOperatorCodes::codeValues() const
{
    return std::vector<WordType> {
        dbl_dbl_code.getValue(), int_dbl_code.getValue(),
        dbl_int_code.getValue(), int_int_code.getValue()
    };
}

// ----------------------------------------

void executeNegateDbl(Executer &executer)
{
    executer.top().dbl_value = -executer.top().dbl_value;
}

void executeNegateInt(Executer &executer)
{
    executer.top().int_value = -executer.top().int_value;
}

OperatorCode<OpType::Dbl> neg_dbl_code {recreateUnaryOperator, executeNegateDbl};
OperatorCode<OpType::Int> neg_int_code {recreateUnaryOperator, executeNegateInt};

UnaryOperatorCodes neg_codes {neg_dbl_code, neg_int_code};

// ----------------------------------------

inline void validatePowerResult(double x, double result, Executer &executer);
inline void calculatePowerDblDbl(Executer &executer, double x, double y);

void executeExponentialDblDbl(Executer &executer)
{
    auto y = executer.top().dbl_value;
    executer.pop();
    auto x = executer.top().dbl_value;
    calculatePowerDblDbl(executer, x, y);
}

void executeExponentialIntDbl(Executer &executer)
{
    auto y = executer.top().dbl_value;
    executer.pop();
    auto x = static_cast<double>(executer.top().int_value);
    calculatePowerDblDbl(executer, x, y);
}

inline void calculatePowerDblDbl(Executer &executer, double x, double y)
{
    auto result = std::pow(x, y);
    validatePowerResult(x, result, executer);
    executer.top().dbl_value = result;
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
    auto y = executer.top().int_value;
    executer.pop();
    auto x = executer.top().dbl_value;
    executer.top().dbl_value = PowerDblInt{executer, x, y}();
}

void executeExponentialIntInt(Executer &executer)
{
    auto y = executer.top().int_value;
    executer.pop();
    auto x = executer.top().int_value;
    executer.top().int_value = PowerIntInt{executer, x, y}();
}

OperatorCode<OpType::DblDbl> exp_dbl_dbl_code {recreateBinaryOperator, executeExponentialDblDbl};
OperatorCode<OpType::IntDbl> exp_int_dbl_code {recreateBinaryOperator, executeExponentialIntDbl};
OperatorCode<OpType::DblInt> exp_dbl_int_code {recreateBinaryOperator, executeExponentialDblInt};
OperatorCode<OpType::IntInt> exp_int_int_code {recreateBinaryOperator, executeExponentialIntInt};

NumOperatorCodes exp_codes {exp_dbl_dbl_code, exp_int_dbl_code, exp_dbl_int_code, exp_int_int_code};

// ----------------------------------------

inline void doDoubleMultiply(Executer &executer, double rhs);
inline void multiplyAndCheckResult(Executer &executer, double lhs, double rhs);

void executeMultiplyDblDbl(Executer &executer)
{
    auto rhs = executer.top().dbl_value;
    doDoubleMultiply(executer, rhs);
}

void executeMultiplyIntDbl(Executer &executer)
{
    auto rhs = executer.top().dbl_value;
    executer.pop();
    auto lhs = static_cast<double>(executer.top().int_value);
    multiplyAndCheckResult(executer, lhs, rhs);
}

void executeMultiplyDblInt(Executer &executer)
{
    auto rhs = static_cast<double>(executer.top().int_value);
    doDoubleMultiply(executer, rhs);
}

inline void doDoubleMultiply(Executer &executer, double rhs)
{
    executer.pop();
    auto lhs = executer.top().dbl_value;
    multiplyAndCheckResult(executer, lhs, rhs);
}

inline void multiplyAndCheckResult(Executer &executer, double lhs, double rhs)
{
    auto result = lhs * rhs;
    checkOverflow(executer, result);
    executer.top().dbl_value = result;
}

void executeMultiplyIntInt(Executer &executer)
{
    auto rhs = executer.top().int_value;
    executer.pop();
    int64_t result = executer.top().int_value;
    result *= rhs;
    checkOverflow(executer, result);
    executer.top().int_value = result;
}

OperatorCode<OpType::DblDbl> mul_dbl_dbl_code {recreateBinaryOperator, executeMultiplyDblDbl};
OperatorCode<OpType::IntDbl> mul_int_dbl_code {recreateBinaryOperator, executeMultiplyIntDbl};
OperatorCode<OpType::DblInt> mul_dbl_int_code {recreateBinaryOperator, executeMultiplyDblInt};
OperatorCode<OpType::IntInt> mul_int_int_code {recreateBinaryOperator, executeMultiplyIntInt};

NumOperatorCodes mul_codes {mul_dbl_dbl_code, mul_int_dbl_code, mul_dbl_int_code, mul_int_int_code};

// ----------------------------------------

template <typename T>
void checkDivideByZero(Executer &executer, T rhs)
{
    if (rhs == 0) {
        throw RunError {"divide by zero", executer.currentOffset()};
    }
}

inline double popDoubleDivisor(Executer &executer);
inline double popIntegerDivisor(Executer &executer);
inline void divideAndCheckResult(Executer &executer, double lhs, double rhs);

void executeDivideDblDbl(Executer &executer)
{
    auto rhs = popDoubleDivisor(executer);
    auto lhs = executer.top().dbl_value;
    divideAndCheckResult(executer, lhs, rhs);
}

void executeDivideIntDbl(Executer &executer)
{
    auto rhs = popDoubleDivisor(executer);
    auto lhs = static_cast<double>(executer.top().int_value);
    divideAndCheckResult(executer, lhs, rhs);
}

inline double popDoubleDivisor(Executer &executer)
{
    auto rhs = executer.top().dbl_value;
    checkDivideByZero(executer, rhs);
    executer.pop();
    return rhs;
}

void executeDivideDblInt(Executer &executer)
{
    auto rhs = static_cast<double>(popIntegerDivisor(executer));
    auto lhs = executer.top().dbl_value;
    auto result = lhs / rhs;
    executer.top().dbl_value = result;
}

inline void divideAndCheckResult(Executer &executer, double lhs, double rhs)
{
    auto result = lhs / rhs;
    checkOverflow(executer, result);
    executer.top().dbl_value = result;
}

void executeDivideIntInt(Executer &executer)
{
    auto rhs = popIntegerDivisor(executer);
    executer.top().int_value = executer.top().int_value / rhs;
}

inline double popIntegerDivisor(Executer &executer)
{
    auto rhs = executer.top().int_value;
    checkDivideByZero(executer, rhs);
    executer.pop();
    return rhs;
}

OperatorCode<OpType::DblDbl> div_dbl_dbl_code {recreateBinaryOperator, executeDivideDblDbl};
OperatorCode<OpType::IntDbl> div_int_dbl_code {recreateBinaryOperator, executeDivideIntDbl};
OperatorCode<OpType::DblInt> div_dbl_int_code {recreateBinaryOperator, executeDivideDblInt};
OperatorCode<OpType::IntInt> div_int_int_code {recreateBinaryOperator, executeDivideIntInt};

NumOperatorCodes div_codes {div_dbl_dbl_code, div_int_dbl_code, div_dbl_int_code, div_int_int_code};
