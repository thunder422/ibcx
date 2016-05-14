/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef PROGRAMWORD_H
#define PROGRAMWORD_H

#include <cstdint>

#include "code.h"


class ProgramWord {
public:
    ProgramWord(Code code);
    ProgramWord(uint16_t word_);

    uint16_t instructionCode() const;

private:
    uint16_t word;
};


inline ProgramWord::ProgramWord(Code code) :
    word {code.getValue()}
{
}

inline ProgramWord::ProgramWord(uint16_t word_) :
    word {word_}
{
}

inline uint16_t ProgramWord::instructionCode() const
{
    return word;
}


#endif // PROGRAMWORD_H
