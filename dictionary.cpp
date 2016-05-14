/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "dictionary.h"

Dictionary::Dictionary()
{
}

uint16_t Dictionary::add(const std::string &string)
{
    int index = key_map.size();
    auto iterator_isnew = key_map.emplace(string, index);
    iterator.emplace_back(iterator_isnew.first);
    return index;
}

std::string Dictionary::get(uint16_t index)
{
    return iterator[index]->first;
}
