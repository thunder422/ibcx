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
    const char *getKeyword(WordType code_value) const;

private:
    struct OperatorData {
        OperatorCodes &codes;
        const char *keyword;
    };

    PrecedenceInfo();
    void initializeOperatorData(const OperatorData &operator_data);

    std::unordered_map<WordType, const char *> keywords;
};

// ------------------------------------------------------------

const char *Precedence::getKeyword(WordType code_value)
{
    return PrecedenceInfo::getInstance().getKeyword(code_value);
}

// ------------------------------------------------------------

PrecedenceInfo &PrecedenceInfo::getInstance()
{
    static PrecedenceInfo precedence_info;
    return precedence_info;
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

    std::initializer_list<OperatorData> operator_data_list = {
        {exp_codes, "^"},
        {neg_codes, "-"}
    };

    for (auto &operator_data : operator_data_list) {
        initializeOperatorData(operator_data);
    }
}

void PrecedenceInfo::initializeOperatorData(const OperatorData &operator_data)
{
    for (auto code_value : operator_data.codes.codeValues()) {
        keywords[code_value] = operator_data.keyword;
    }
}
