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

Dictionary::Entry Dictionary::add(const std::string &string)
{
    auto key_map_entry = addToKeyMap(string);
    auto entry = addToKeyIteratorVector(key_map_entry);
    return entry;
}

Dictionary::KeyMapEntry Dictionary::addToKeyMap(const std::string &string)
{
    auto index = static_cast<WordType>(key_map.size());
    auto emplace_result = key_map.emplace(string, index);
    return KeyMapEntry {emplace_result.first, !emplace_result.second};
}

Dictionary::Entry Dictionary::addToKeyIteratorVector(const Dictionary::KeyMapEntry &key_map_entry)
{
    if (!key_map_entry.key_exists) {
        key_iterator.emplace_back(key_map_entry.iterator);
    }
    auto entry_value = key_map_entry.iterator->second;
    auto operand = entry_value.index;
    return Entry {operand, key_map_entry.key_exists};
}

std::string Dictionary::get(WordType index) const
{
    return key_iterator[index]->first;
}
