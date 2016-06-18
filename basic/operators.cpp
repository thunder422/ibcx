/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "code.h"
#include "executer.h"
#include "operators.h"
#include "recreator.h"


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

OperatorCode<OpType::DblDbl> exp_dbl_dbl_code {nullptr, nullptr};
OperatorCode<OpType::IntDbl> exp_int_dbl_code {nullptr, nullptr};
OperatorCode<OpType::DblInt> exp_dbl_int_code {nullptr, nullptr};
OperatorCode<OpType::IntInt> exp_int_int_code {nullptr, nullptr};

NumOperatorCodes exp_codes {exp_dbl_dbl_code, exp_int_dbl_code, exp_dbl_int_code, exp_int_int_code};
