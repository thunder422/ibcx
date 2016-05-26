/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_PROGRAMWORD_H
#define IBC_PROGRAMWORD_H

#include <cstdint>

#include "code.h"


class ProgramWord {
public:
    explicit ProgramWord(Code code);
    explicit ProgramWord(uint16_t word);

    uint16_t instructionCode() const;
    uint16_t operand() const;

private:
    uint16_t word;
};


inline ProgramWord::ProgramWord(Code code) :
    word {code.getValue()}
{
}

inline ProgramWord::ProgramWord(uint16_t word) :
    word {word}
{
}

inline uint16_t ProgramWord::instructionCode() const
{
    return word;
}

inline uint16_t ProgramWord::operand() const
{
    return word;
}


#endif  // IBC_PROGRAMWORD_H
