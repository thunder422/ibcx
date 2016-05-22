/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_COMMANDPARSER_H
#define IBC_COMMANDPARSER_H

#include <iosfwd>
#include <memory>


class ProgramCode;
class ProgramUnit;

class CommandParser {
public:
    CommandParser(std::istream &is, ProgramCode &code_line, ProgramUnit &program);
    void operator()();
    ~CommandParser();

private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};


#endif  // IBC_COMMANDPARSER_H
