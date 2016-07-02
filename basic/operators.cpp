/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <cmath>
#include <limits>

#include "code.h"
#include "executer.h"
#include "operators.h"
#include "recreator.h"
#include "runerror.h"


UnaryOperatorCodes::UnaryOperatorCodes(OperatorCode<OpType::Dbl> &dbl_code,
        OperatorCode<OpType::Int> &int_code) :
    dbl_code {dbl_code},
    int_code {int_code}
{
}

Code &UnaryOperatorCodes::select(DataType data_type) const
{
    if (data_type == DataType::Double) {
        return dbl_code;
    } else {
        return int_code;
    }
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

// ----------------------------------------

void recreateNegate(Recreator &recreator);
void executeNegateDbl(Executer &executer);
void executeNegateInt(Executer &executer);

OperatorCode<OpType::Dbl> neg_dbl_code {recreateNegate, executeNegateDbl};
OperatorCode<OpType::Int> neg_int_code {recreateNegate, executeNegateInt};

UnaryOperatorCodes neg_codes {neg_dbl_code, neg_int_code};


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

// ----------------------------------------

void recreateExponential(Recreator &recreator);
void executeExponentialIntInt(Executer &executer);

OperatorCode<OpType::DblDbl> exp_dbl_dbl_code {recreateExponential, nullptr};
OperatorCode<OpType::IntDbl> exp_int_dbl_code {recreateExponential, nullptr};
OperatorCode<OpType::DblInt> exp_dbl_int_code {recreateExponential, nullptr};
OperatorCode<OpType::IntInt> exp_int_int_code {recreateExponential, executeExponentialIntInt};

NumOperatorCodes exp_codes {exp_dbl_dbl_code, exp_int_dbl_code, exp_dbl_int_code, exp_int_int_code};

void recreateExponential(Recreator &recreator)
{
    std::string string = recreator.top();
    recreator.pop();
    recreator.append(' ');
    recreator.markErrorStart();
    recreator.append('^');
    recreator.append(' ');
    recreator.append(string);
}

struct PowerIntInt {
    PowerIntInt(int x, int y);
    int operator()();

private:
    int calculateNegativeExponent();
    int calculatePower();
    int caculateForPositiveValue();
    int multiplyPositiveValue();
    int useDoublePowerForPositiveValue();
    int calculateForNegativeValue();
    int multiplyNegativeValue();
    int useDoublePowerForNegativeValue();

    int x;
    int y;
};

void executeExponentialIntInt(Executer &executer)
{
    auto y = executer.top().int_value;
    executer.pop();
    auto x = executer.top().int_value;
    try {
        executer.top().int_value = PowerIntInt{x, y}();
    }
    catch (RunError &error) {
        error.offset = executer.currentOffset();
        throw;
    }
}

inline PowerIntInt::PowerIntInt(int x, int y) :
    x {x},
    y {y}
{
}

inline int PowerIntInt::operator()()
{
    if (y < 0) {
        if (x == 0) {
            throw RunError {"divide by zero", 0};
        }
        return calculateNegativeExponent();
    }
    return calculatePower();
}

inline int PowerIntInt::calculateNegativeExponent()
{
    if (x == 1) {
        return 1;
    } else if (x == -1) {
        return (y & 1) ? -1 : 1;
    } else {
        return 0;
    }
}

inline int PowerIntInt::calculatePower()
{
    return x >= 0 ? caculateForPositiveValue() : calculateForNegativeValue();
}

inline int PowerIntInt::caculateForPositiveValue()
{
    return y < 19 ? multiplyPositiveValue() : useDoublePowerForPositiveValue();
}

inline int PowerIntInt::multiplyPositiveValue()
{
    int64_t result = 1;
    for (int i = 0; i < y; ++i) {
        result *= x;
        if (result > std::numeric_limits<int>::max()) {
            throw RunError {"overflow", 0};
        }
    }
    return result;
}

inline int PowerIntInt::useDoublePowerForPositiveValue()
{
    double result = std::pow(x, y);
    if (result > std::numeric_limits<int>::max()) {
        throw RunError {"overflow", 0};
    }
    return result;
}

inline int PowerIntInt::calculateForNegativeValue()
{
    return y < 17 ? multiplyNegativeValue() : useDoublePowerForNegativeValue();
}

inline int PowerIntInt::multiplyNegativeValue()
{
    int64_t result = 1;
    for (int i = 0; i < y; ++i) {
        result *= x;
        if (result > std::numeric_limits<int>::max() || result < std::numeric_limits<int>::min()) {
            throw RunError {"overflow", 0};
        }
    }
    return result;
}

inline int PowerIntInt::useDoublePowerForNegativeValue()
{
    double result = std::pow(x, y);
    return result;
}
