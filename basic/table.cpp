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
#include "table.h"


class TableInfo {
public:
    static TableInfo &getInstance();
    void addOperatorData(Precedence precedence, Codes &codes, const char *keyword);
    void addNumFunctionData(Codes &codes, const char *keyword);
    Precedence getPrecedence(WordType code_value) const;
    const char *getKeyword(WordType code_value) const;
    Codes *operatorCodes(Precedence precedence);
    Codes *operatorCodes(Precedence precedence, char operator_char);
    Codes *operatorCodes(Precedence precedence, const ci_string &word);
    ComparisonOperator comparisonOperatorData(const std::string &keyword);
    Codes *numFunctionCodes(const ci_string &word);

private:
    struct OperatorData {
        Codes &codes;
        const char *keyword;
    };

    TableInfo() { }

    std::unordered_map<WordType, Precedence> precedences;
    std::unordered_map<WordType, const char *> keywords;
    std::multimap<Precedence, OperatorData> operator_data;
    std::map<std::string, ComparisonOperator> comparison_operator_data;
    std::map<ci_string, Codes &> num_function_codes;
};

// ------------------------------------------------------------

void Table::addOperatorCodes(Precedence precedence, Codes &codes, const char *keyword)
{
    TableInfo::getInstance().addOperatorData(precedence, codes, keyword);
}

void Table::addNumFunctionCodes(Codes &codes, const char *keyword)
{
    TableInfo::getInstance().addNumFunctionData(codes, keyword);
}

const char *Table::getKeyword(WordType code_value)
{
    return TableInfo::getInstance().getKeyword(code_value);
}

Precedence Table::getPrecedence(WordType code_value)
{
    return TableInfo::getInstance().getPrecedence(code_value);
}

Codes *Table::operatorCodes(Precedence precedence)
{
    return TableInfo::getInstance().operatorCodes(precedence);
}

Codes *Table::operatorCodes(Precedence precedence, char operator_char)
{
    return TableInfo::getInstance().operatorCodes(precedence, operator_char);
}

Codes *Table::operatorCodes(Precedence precedence, const ci_string &word)
{
    return TableInfo::getInstance().operatorCodes(precedence, word);
}

ComparisonOperator Table::comparisonOperator(const std::string &keyword)
{
    return TableInfo::getInstance().comparisonOperatorData(keyword);
}

Codes *Table::numFunctionCodes(const ci_string &word)
{
    return TableInfo::getInstance().numFunctionCodes(word);
}

// ------------------------------------------------------------

TableInfo &TableInfo::getInstance()
{
    static TableInfo precedence_info;
    return precedence_info;
}

Precedence TableInfo::getPrecedence(WordType code_value) const
{
    return precedences.at(code_value);
}

const char *TableInfo::getKeyword(WordType code_value) const
{
    return keywords.at(code_value);
}

Codes *TableInfo::operatorCodes(Precedence precedence)
{
    auto iterators = operator_data.equal_range(precedence);
    auto &operator_data = iterators.first->second;
    return &operator_data.codes;
}

Codes *TableInfo::operatorCodes(Precedence precedence, char operator_char)
{
    auto iterators = operator_data.equal_range(precedence);
    for (auto iterator = iterators.first; iterator != iterators.second; ++iterator) {
        auto &operator_data = iterator->second;
        if (operator_char == operator_data.keyword[0]) {
            return &operator_data.codes;
        }
    }
    return nullptr;
}

Codes *TableInfo::operatorCodes(Precedence precedence, const ci_string &word)
{
    auto iterators = operator_data.equal_range(precedence);
    for (auto iterator = iterators.first; iterator != iterators.second; ++iterator) {
        auto &operator_data = iterator->second;
        if (word == operator_data.keyword) {
            return &operator_data.codes;
        }
    }
    return nullptr;
}

ComparisonOperator TableInfo::comparisonOperatorData(const std::string &keyword)
{
    auto iterator = comparison_operator_data.find(keyword);
    if (iterator == comparison_operator_data.end()) {
        return ComparisonOperator {};
    } else {
        return iterator->second;
    }
}

Codes *TableInfo::numFunctionCodes(const ci_string &word)
{
    auto iterator = num_function_codes.find(word);
    return iterator != num_function_codes.end() ? &iterator->second : nullptr;
}

// --------------------

void TableInfo::addOperatorData(Precedence precedence, Codes &codes, const char *keyword)
{
    operator_data.emplace(precedence, OperatorData{codes, keyword});
    for (auto code_value : codes.codeValues()) {
        precedences[code_value] = precedence;
        keywords[code_value] = keyword;
    }
    if (precedence == Precedence::Relation || precedence == Precedence::Equality) {
        comparison_operator_data.emplace(keyword, ComparisonOperator{&codes, precedence});
    }
}

void TableInfo::addNumFunctionData(Codes &codes, const char *keyword)
{
    num_function_codes.emplace(keyword, codes);
    for (auto code_value : codes.codeValues()) {
        keywords[code_value] = keyword;
    }
}
