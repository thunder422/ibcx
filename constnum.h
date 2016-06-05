/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_CONSTNUM_H
#define IBC_CONSTNUM_H

#include "datatype.h"
#include "dictionary.h"
#include "wordtype.h"


struct ConstNumCodeInfo;
class ConstNumConverter;

class ConstNumDictionary : public Dictionary {
public:
    ConstNumCodeInfo add(bool floating_point, const std::string &number);
    const double *getDblValues() const;
    const int *getIntValues() const;

private:
    WordType addToDictionary(const ConstNumConverter &converter, const std::string &number);

    std::vector<double> dbl_values;
    std::vector<int> int_values;
};

inline const double *ConstNumDictionary::getDblValues() const
{
    return dbl_values.data();
}

inline const int *ConstNumDictionary::getIntValues() const
{
    return int_values.data();
}


class Code;

struct ConstNumCodeInfo {
    WordType code_value;
    WordType operand;
    DataType data_type;
};


#endif  // IBC_CONSTNUM_H
