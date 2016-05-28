/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_CISTRING_H
#define IBC_CISTRING_H

#include <cctype>
#include <iostream>
#include <string>

struct ci_char_traits : public std::char_traits<char> {
    static bool eq(char c1, char c2);
    static bool lt(char c1, char c2);
    static bool gt(char c1, char c2);
    static int compare(const char *s1, const char *s2, size_t n);
    static const char *find(const char *s, int n, char c);
};

inline bool ci_char_traits::eq(char c1, char c2)
{
    return std::tolower(c1) == std::tolower(c2);
}

inline bool ci_char_traits::lt(char c1, char c2)
{
    return std::tolower(c1) < std::tolower(c2);
}

inline bool ci_char_traits::gt(char c1, char c2)
{
    return std::tolower(c1) > std::tolower(c2);
}

inline int ci_char_traits::compare(const char *s1, const char *s2, size_t n)
{
    while (n-- != 0) {
        if (lt(*s1, *s2)) {
            return -1;
        }
        if (gt(*s1, *s2)) {
            return 1;
        }
        ++s1;
        ++s2;
    }
    return 0;
}

inline const char *ci_char_traits::find(const char *s, int n, char c)
{
    while (n-- > 0 && !eq(*s, c)) {
        ++s;
    }
    return n >= 0 ? s : 0;
}


typedef std::basic_string<char, ci_char_traits> ci_string;


inline bool operator==(const ci_string &lhs, const std::string &rhs)
{
    return lhs == rhs.c_str();
}

inline bool operator!=(const ci_string &lhs, const std::string &rhs)
{
    return lhs != rhs.c_str();
}

inline bool operator<(const ci_string &lhs, const std::string &rhs)
{
    return lhs < rhs.c_str();
}

inline bool operator>(const ci_string &lhs, const std::string &rhs)
{
    return lhs > rhs.c_str();
}

inline bool operator<=(const ci_string &lhs, const std::string &rhs)
{
    return lhs <= rhs.c_str();
}

inline bool operator>=(const ci_string &lhs, const std::string &rhs)
{
    return lhs >= rhs.c_str();
}

inline bool operator==(const std::string &lhs, const ci_string &rhs)
{
    return lhs.c_str() == rhs;
}

inline bool operator!=(const std::string &lhs, const ci_string &rhs)
{
    return lhs.c_str() != rhs;
}

inline bool operator<(const std::string &lhs, const ci_string &rhs)
{
    return lhs.c_str() < rhs;
}

inline bool operator>(const std::string &lhs, const ci_string &rhs)
{
    return lhs.c_str() > rhs;
}

inline bool operator<=(const std::string &lhs, const ci_string &rhs)
{
    return lhs.c_str() <= rhs;
}

inline bool operator>=(const std::string &lhs, const ci_string &rhs)
{
    return lhs.c_str() >= rhs;
}


inline std::ostream &operator<<(std::ostream &os, const ci_string &cistring)
{
    return os << cistring.c_str();
}

inline std::istream &operator>>(std::istream &is, ci_string &cistring)
{
    std::string string;
    is >> string;
    cistring = string.c_str();
    return is;
}


#endif // IBC_CISTRING_H
