/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_PROGRAMCODE_H
#define IBC_PROGRAMCODE_H

#include <vector>

#include "programword.h"


using ProgramVector = std::vector<ProgramWord>;
using ProgramReference = ProgramVector::reference;
using ProgramConstIterator = ProgramVector::const_iterator;


class ProgramCode {
public:
    ProgramCode();

    bool empty() const;
    std::size_t size() const;
    ProgramReference operator[](std::size_t index);
    template <typename... Args> void emplace_back(Args &&... args);
    void append(ProgramCode &more);
    ProgramConstIterator begin() const;

private:
    ProgramVector code;
};


inline bool ProgramCode::empty() const
{
    return code.empty();
}

inline std::size_t ProgramCode::size() const
{
    return code.size();
}

inline ProgramReference ProgramCode::operator[](std::size_t index)
{
    return code[index];
}

template <typename... Args>
inline void ProgramCode::emplace_back(Args &&... args)
{
    code.emplace_back(std::forward<Args>(args)...);
}

inline ProgramConstIterator ProgramCode::begin() const
{
    return code.cbegin();
}


#endif  // IBC_PROGRAMCODE_H
