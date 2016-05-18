/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <cstdint>
#include <unordered_map>
#include <vector>


class Dictionary {
public:
    Dictionary();
    uint16_t add(const std::string &string);
    std::string get(uint16_t index);

private:
    struct EntryValue {
        EntryValue(uint16_t index);

        uint16_t index;
    };

    using KeyMap = std::unordered_map<std::string, EntryValue>;

    KeyMap key_map;
    std::vector<KeyMap::iterator> iterator;
};


inline Dictionary::EntryValue::EntryValue(uint16_t index) :
    index {index}
{
}


#endif  // DICTIONARY_H
