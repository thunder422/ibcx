/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "functions.h"


bool FunctionCodes::argumentOptional() const
{
    return false;
}

// ----------------------------------------

MultiTypeFunctionCodes::MultiTypeFunctionCodes(const char *keyword,
        FunctionCode<ArgType::Dbl> &dbl_code, FunctionCode<ArgType::Int> &int_code) :
    dbl_code {dbl_code},
    int_code {int_code}
{
    Table::addNumFunctionCodes(*this, keyword);
}

std::vector<WordType> MultiTypeFunctionCodes::codeValues() const
{
    return std::vector<WordType> {dbl_code.getValue(), int_code.getValue()};
}

DataType MultiTypeFunctionCodes::argumentDataType() const
{
    return {};
}

FunctionCodes::Info MultiTypeFunctionCodes::select(std::vector<DataType> argument_data_types) const
{
    if (argument_data_types.front().isDouble()) {
        return FunctionCodes::Info {dbl_code, DataType::Double()};
    } else {
        return FunctionCodes::Info {int_code, DataType::Integer()};
    }
}

// ----------------------------------------

MathFunctionCodes::MathFunctionCodes(const char *keyword, FunctionCode<ArgType::Dbl> &code) :
    code {code}
{
    Table::addNumFunctionCodes(*this, keyword);
}

std::vector<WordType> MathFunctionCodes::codeValues() const
{
    return std::vector<WordType> {code.getValue()};
}

DataType MathFunctionCodes::argumentDataType() const
{
    return DataType::Double();
}

FunctionCodes::Info MathFunctionCodes::select(std::vector<DataType> unused_data_types) const
{
    (void)unused_data_types;
    return FunctionCodes::Info {code, DataType::Double()};
}

// ----------------------------------------

ConvertFunctionCodes::ConvertFunctionCodes(const char *keyword, FunctionCode<ArgType::Int> &code) :
    code {code},
    argument_data_type {DataType::Integer()},
    return_data_type {DataType::Double()}
{
    Table::addNumFunctionCodes(*this, keyword);
}

ConvertFunctionCodes::ConvertFunctionCodes(const char *keyword, FunctionCode<ArgType::Dbl> &code) :
    code {code},
    argument_data_type {DataType::Double()},
    return_data_type {DataType::Integer()}
{
    Table::addNumFunctionCodes(*this, keyword);
}

std::vector<WordType> ConvertFunctionCodes::codeValues() const
{
    return std::vector<WordType> {code.getValue()};
}

DataType ConvertFunctionCodes::argumentDataType() const
{
    return argument_data_type;
}

FunctionCodes::Info ConvertFunctionCodes::select(std::vector<DataType> unused_data_types) const
{
    (void)unused_data_types;
    return FunctionCodes::Info {code, return_data_type};
}

// ----------------------------------------

RandomFunctionCodes::RandomFunctionCodes(const char *keyword,
        FunctionCode<ArgType::None> &none_code, FunctionCode<ArgType::Int> &int_code) :
    none_code {none_code},
    int_code {int_code}
{
    Table::addNumFunctionCodes(*this, keyword);
}

std::vector<WordType> RandomFunctionCodes::codeValues() const
{
    return std::vector<WordType> {none_code.getValue(), int_code.getValue()};
}

bool RandomFunctionCodes::argumentOptional() const
{
    return true;
}

DataType RandomFunctionCodes::argumentDataType() const
{
    return {};
}

FunctionCodes::Info RandomFunctionCodes::select(std::vector<DataType> argument_data_types) const
{
    if (argument_data_types.empty()) {
        return FunctionCodes::Info {none_code, DataType::Double()};
    } else {
        return FunctionCodes::Info {int_code, DataType::Integer()};
    }
}
