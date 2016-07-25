/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <map>
#include <unordered_map>

#include "cistring.h"
#include "operators.h"
#include "precedence.h"


class PrecedenceInfo {
public:
    static PrecedenceInfo &getInstance();
    Precedence::Level getPrecedence(WordType code_value) const;
    const char *getKeyword(WordType code_value) const;
    OperatorCodes *operatorCodes(Precedence::Level precedence);
    OperatorCodes *operatorCodes(Precedence::Level precedence, char operator_char);
    OperatorCodes *operatorCodes(Precedence::Level precedence, const ci_string &word);

private:
    struct OperatorData {
        Precedence::Level precedence;
        OperatorCodes &codes;
        const char *keyword;
    };

    PrecedenceInfo();
    void initializeOperatorData(const OperatorData &operator_data);

    std::unordered_map<WordType, Precedence::Level> precedences;
    std::unordered_map<WordType, const char *> keywords;
    std::multimap<Precedence::Level, const OperatorData *> operator_data_map;
};

// ------------------------------------------------------------

const char *Precedence::getKeyword(WordType code_value)
{
    return PrecedenceInfo::getInstance().getKeyword(code_value);
}

Precedence::Level Precedence::getPrecedence(WordType code_value)
{
    return PrecedenceInfo::getInstance().getPrecedence(code_value);
}

OperatorCodes *Precedence::operatorCodes(Precedence::Level precedence)
{
    return PrecedenceInfo::getInstance().operatorCodes(precedence);
}

OperatorCodes *Precedence::operatorCodes(Precedence::Level precedence, char operator_char)
{
    return PrecedenceInfo::getInstance().operatorCodes(precedence, operator_char);
}

OperatorCodes *Precedence::operatorCodes(Precedence::Level precedence, const ci_string &word)
{
    return PrecedenceInfo::getInstance().operatorCodes(precedence, word);
}

// ------------------------------------------------------------

PrecedenceInfo &PrecedenceInfo::getInstance()
{
    static PrecedenceInfo precedence_info;
    return precedence_info;
}

Precedence::Level PrecedenceInfo::getPrecedence(WordType code_value) const
{
    return precedences.at(code_value);
}

const char *PrecedenceInfo::getKeyword(WordType code_value) const
{
    return keywords.at(code_value);
}

OperatorCodes *PrecedenceInfo::operatorCodes(Precedence::Level precedence)
{
    auto iterators = operator_data_map.equal_range(precedence);
    auto operator_data = iterators.first->second;
    return &operator_data->codes;
}

OperatorCodes *PrecedenceInfo::operatorCodes(Precedence::Level precedence, char operator_char)
{
    auto iterators = operator_data_map.equal_range(precedence);
    for (auto iterator = iterators.first; iterator != iterators.second; ++iterator) {
        auto *operator_data = iterator->second;
        if (operator_char == operator_data->keyword[0]) {
            return &operator_data->codes;
        }
    }
    return nullptr;
}

OperatorCodes *PrecedenceInfo::operatorCodes(Precedence::Level precedence, const ci_string &word)
{
    auto iterators = operator_data_map.equal_range(precedence);
    for (auto iterator = iterators.first; iterator != iterators.second; ++iterator) {
        auto *operator_data = iterator->second;
        if (word == operator_data->keyword) {
            return &operator_data->codes;
        }
    }
    return nullptr;
}

// --------------------

PrecedenceInfo::PrecedenceInfo()
{
    extern NumOperatorCodes exp_codes;
    extern UnaryOperatorCodes neg_codes;
    extern NumOperatorCodes mul_codes;
    extern NumOperatorCodes div_codes;
    extern IntDivOperatorCode int_div_codes;
    extern NumOperatorCodes mod_codes;
    extern NumOperatorCodes add_codes;
    extern NumOperatorCodes sub_codes;

    static std::initializer_list<OperatorData> operator_data_list = {
        {Precedence::Exponential, exp_codes, "^"},
        {Precedence::Negate, neg_codes, "-"},
        {Precedence::Product, mul_codes, "*"},
        {Precedence::Product, div_codes, "/"},
        {Precedence::IntDivide, int_div_codes, "\\"},
        {Precedence::Modulo, mod_codes, "MOD"},
        {Precedence::Summation, add_codes, "+"},
        {Precedence::Summation, sub_codes, "-"}
    };

    for (auto &operator_data : operator_data_list) {
        initializeOperatorData(operator_data);
    }
}

void PrecedenceInfo::initializeOperatorData(const OperatorData &operator_data)
{
    auto precedence = operator_data.precedence;
    operator_data_map.emplace(precedence, &operator_data);
    for (auto code_value : operator_data.codes.codeValues()) {
        precedences[code_value] = precedence;
        keywords[code_value] = operator_data.keyword;
    }
}
