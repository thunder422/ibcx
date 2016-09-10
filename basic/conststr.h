/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_CONSTSTR_H
#define IBC_CONSTSTR_H

#include <memory>

#include "dictionary.h"


class ConstStrDictionary : public Dictionary {
public:
    WordType add(const std::string &string);
    const std::unique_ptr<std::string> *getStrValues() const;

private:
    std::vector<std::unique_ptr<std::string>> str_values;
};

inline const std::unique_ptr<std::string> *ConstStrDictionary::getStrValues() const
{
    return str_values.data();
}


#endif  // IBC_CONSTSTR_H
