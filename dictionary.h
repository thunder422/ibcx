/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_DICTIONARY_H
#define IBC_DICTIONARY_H

#include <unordered_map>
#include <vector>

#include "wordtype.h"


class Dictionary {
public:
    struct Entry {
        WordType operand;
        bool exists;
    };

    Dictionary();
    Entry add(const std::string &string);
    std::string get(WordType index);

private:
    struct EntryValue {
        EntryValue(WordType index);

        WordType index;
    };

    using KeyMap = std::unordered_map<std::string, EntryValue>;
    struct KeyMapEntry {
        KeyMap::iterator iterator;
        bool key_exists;
    };
    Dictionary::KeyMapEntry addToKeyMap(const std::string &string);
    Entry addToKeyIteratorVector(const Dictionary::KeyMapEntry &key_map_entry);

    KeyMap key_map;
    std::vector<KeyMap::iterator> key_iterator;
};


inline Dictionary::EntryValue::EntryValue(WordType index) :
    index {index}
{
}


#endif  // IBC_DICTIONARY_H
