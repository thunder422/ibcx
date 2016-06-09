/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_PROGRAMWORD_H
#define IBC_PROGRAMWORD_H

#include "code.h"
#include "wordtype.h"


class ProgramWord {
public:
    explicit ProgramWord(Code code);
    explicit ProgramWord(WordType word);

    Code *instructionCode() const;
    WordType operand() const;

    const WordType *addressOf() const;

private:
    WordType word;
};


inline ProgramWord::ProgramWord(Code code) :
    word {code.getValue()}
{
}

inline ProgramWord::ProgramWord(WordType word) :
    word {word}
{
}

inline Code *ProgramWord::instructionCode() const
{
    return Code::getCode(word);
}

inline WordType ProgramWord::operand() const
{
    return word;
}

inline const WordType *ProgramWord::addressOf() const
{
    return &word;
}


#endif  // IBC_PROGRAMWORD_H
