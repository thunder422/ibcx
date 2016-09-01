/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_FUNCTIONS_H
#define IBC_FUNCTIONS_H

#include "code.h"
#include "codes.h"
#include "table.h"


enum class ArgType {
    Dbl,
    Int,
    None
};


template <ArgType arg_type>
class FunctionCode : public Code {
public:
    FunctionCode(RecreateFunctionPointer recreate_function,
            ExecuteFunctionPointer execute_function) :
        Code(recreate_function, execute_function) { }
};


class FunctionCodes : public Codes {
public:
    struct Info {
        Code &code;
        DataType result_data_type;
    };

    virtual bool argumentOptional() const;
    virtual DataType argumentDataType() const = 0;
    virtual Info select(DataType data_type) const = 0;
};


class MultiTypeFunctionCodes : public FunctionCodes {
public:
    MultiTypeFunctionCodes(const char *keyword, FunctionCode<ArgType::Dbl> &dbl_code,
        FunctionCode<ArgType::Int> &int_code);
    std::vector<WordType> codeValues() const override;
    Info select(DataType data_type) const override;
    DataType argumentDataType() const override;

private:
    FunctionCode<ArgType::Dbl> &dbl_code;
    FunctionCode<ArgType::Int> &int_code;
};


class MathFunctionCodes : public FunctionCodes {
public:
    MathFunctionCodes(const char *keyword, FunctionCode<ArgType::Dbl> &code);
    std::vector<WordType> codeValues() const override;
    DataType argumentDataType() const override;
    Info select(DataType unused_data_type) const override;

private:
    FunctionCode<ArgType::Dbl> &code;
};


class ConvertFunctionCodes : public FunctionCodes {
public:
    ConvertFunctionCodes(const char *keyword, FunctionCode<ArgType::Int> &code);
    ConvertFunctionCodes(const char *keyword, FunctionCode<ArgType::Dbl> &code);
    std::vector<WordType> codeValues() const override;
    DataType argumentDataType() const override;
    Info select(DataType data_type) const override;

private:
    Code &code;
    DataType argument_data_type;
    DataType return_data_type;
};


class RandomFunctionCodes : public FunctionCodes {
public:
    RandomFunctionCodes(const char *keyword, FunctionCode<ArgType::None> &none_code,
        FunctionCode<ArgType::Int> &int_code);
    std::vector<WordType> codeValues() const override;
    bool argumentOptional() const override;
    DataType argumentDataType() const override;
    Info select(DataType data_type) const override;


private:
    FunctionCode<ArgType::None> &none_code;
    FunctionCode<ArgType::Int> &int_code;
};


#endif  // IBC_FUNCTIONS_H
