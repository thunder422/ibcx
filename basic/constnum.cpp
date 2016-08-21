/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "code.h"
#include "compileerror.h"
#include "constnum.h"
#include "executer.h"
#include "overflow.h"
#include "recreator.h"


void recreateConstNum(Recreator &recreator)
{
    auto number = recreator.getConstNumOperand();
    recreator.push(number);
}

void executeConstDbl(Executer &executer)
{
    auto operand = executer.getOperand();
    executer.pushConstDbl(operand);
}

void executeConstInt(Executer &executer)
{
    auto operand = executer.getOperand();
    executer.pushConstInt(operand);
}

Code const_dbl_code {recreateConstNum, executeConstDbl};
Code const_int_code {recreateConstNum, executeConstInt};

class ConstNumConverter {
public:
    ConstNumConverter(bool floating_point, const std::string &number);

    ConstNumCodeInfo convertNumber();
    double getDblValue() const;
    int32_t getIntValue() const;

private:
    void tryToCreateIntCode();
    void createIntCode();
    void createDblCode();

    bool floating_point;
    const std::string &number;
    ConstNumCodeInfo const_num_code_info;
    double dbl_value;
    int32_t int_value;
};

ConstNumCodeInfo ConstNumDictionary::add(bool floating_point, const std::string &number)
{
    ConstNumConverter converter {floating_point, number};
    auto const_num_code_info = converter.convertNumber();
    const_num_code_info.operand = addToDictionary(converter, number);
    return const_num_code_info;
}

bool ConstNumDictionary::convertibleToInteger(WordType index) const
{
    return withinIntegerRange(dbl_values[index]);
}

WordType ConstNumDictionary::addToDictionary(const ConstNumConverter &converter,
    const std::string &number)
{
    auto entry = Dictionary::add(number);
    if (!entry.exists) {
        dbl_values.push_back(converter.getDblValue());
        int_values.push_back(converter.getIntValue());
    }
    return entry.operand;
}

// ----------------------------------------

ConstNumConverter::ConstNumConverter(bool floating_point, const std::string &number) :
    floating_point {floating_point},
    number {number}
{
}

ConstNumCodeInfo ConstNumConverter::convertNumber()
{
    if (!floating_point) {
        tryToCreateIntCode();
    }
    if (floating_point) {
        createDblCode();
    }
    return const_num_code_info;
}

void ConstNumConverter::tryToCreateIntCode()
{
    try {
        createIntCode();
    }
    catch (const std::out_of_range &) {
        floating_point = true;
    }
}

void ConstNumConverter::createIntCode()
{
    int_value = std::stoi(number);
    const_num_code_info.code_value = const_int_code.getValue();
    const_num_code_info.data_type = DataType::Integer;
    dbl_value = int_value;
}

void ConstNumConverter::createDblCode()
{
    dbl_value = std::stod(number);
    const_num_code_info.code_value = const_dbl_code.getValue();
    const_num_code_info.data_type = DataType::Double;
    int_value = dbl_value;  // FIXME - may not be possible
}

double ConstNumConverter::getDblValue() const
{
    return dbl_value;
}

int32_t ConstNumConverter::getIntValue() const
{
    return int_value;
}
