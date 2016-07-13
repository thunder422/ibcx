/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <unordered_map>

#include "operators.h"
#include "precedence.h"


class PrecedenceInfo {
public:
    static PrecedenceInfo &getInstance();
    Precedence::Level getPrecedence(WordType code_value) const;
    const char *getKeyword(WordType code_value) const;

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

// --------------------

PrecedenceInfo::PrecedenceInfo()
{
    extern NumOperatorCodes exp_codes;
    extern UnaryOperatorCodes neg_codes;
    extern NumOperatorCodes mul_codes;

    std::initializer_list<OperatorData> operator_data_list = {
        {Precedence::Exponential, exp_codes, "^"},
        {Precedence::Negate, neg_codes, "-"},
        {Precedence::Product, mul_codes, "*"}
    };

    for (auto &operator_data : operator_data_list) {
        initializeOperatorData(operator_data);
    }
}

void PrecedenceInfo::initializeOperatorData(const OperatorData &operator_data)
{
    auto precedence = operator_data.precedence;
    for (auto code_value : operator_data.codes.codeValues()) {
        precedences[code_value] = precedence;
        keywords[code_value] = operator_data.keyword;
    }
}
