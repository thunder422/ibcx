/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef PROGRAMCODE_H
#define PROGRAMCODE_H

#include <vector>

#include "programword.h"


using ProgramVector = std::vector<ProgramWord>;


class ProgramCode {
public:
    ProgramCode();

    bool empty() const;
    std::size_t size() const;
    template <typename... Args> void emplace_back(Args&&... args);

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

template <typename... Args>
inline void ProgramCode::emplace_back(Args&&... args)
{
    code.emplace_back(std::forward<Args>(args)...);
}


#endif  // PROGRAMCODE_H
