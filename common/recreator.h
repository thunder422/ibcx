/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_RECREATOR_H
#define IBC_RECREATOR_H

#include <memory>
#include <string>


class CommandCode;
class ProgramReader;
class ProgramUnit;

class Recreator {
public:
    static std::unique_ptr<Recreator> create(const ProgramUnit &program,
        ProgramReader program_reader, unsigned error_offset);

    virtual ~Recreator() = default;

    virtual std::string &&recreate() = 0;
    virtual std::string getConstNumOperand() const = 0;
    virtual void addCommandKeyword(CommandCode command_code) = 0;
    virtual void push(const std::string &operand) = 0;

    virtual void recreateUnaryOperator() = 0;
    virtual void recreateBinaryOperator() = 0;
    virtual void recreateFunction() = 0;
    virtual void markOperandIfError() = 0;
};


void recreateUnaryOperator(Recreator &recreator);
void recreateBinaryOperator(Recreator &recreator);
void recreateFunction(Recreator &recreator);
void recreateNothing(Recreator &recreator);


#endif  // IBC_RECREATOR_H
