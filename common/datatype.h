/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_DATATYPE_H
#define IBC_DATATYPE_H


class DataType {
public:
    static DataType Double();
    static DataType Integer();
    static DataType String();

    DataType();
    explicit operator bool() const;
    bool isDouble() const;
    bool isInteger() const;
    bool isString() const;
    bool isNumeric() const;
    bool isNotNumeric() const;

private:
    enum class Enum {
        Null,
        Double,
        Integer,
        String
    };

    DataType(Enum value);

    Enum value;
};


inline DataType::DataType(Enum value) :
    value {value}
{
}

inline DataType DataType::Double()
{
    return DataType {Enum::Double};
}

inline DataType DataType::Integer()
{
    return DataType {Enum::Integer};
}

inline DataType DataType::String()
{
    return DataType {Enum::String};
}

inline DataType::DataType() :
    value {Enum::Null}
{
}

inline DataType::operator bool() const
{
    return value != Enum::Null;
}

inline bool DataType::isDouble() const
{
    return value == Enum::Double;
}

inline bool DataType::isInteger() const
{
    return value == Enum::Integer;
}

inline bool DataType::isString() const
{
    return value == Enum::String;
}

inline bool DataType::isNumeric() const
{
    return isDouble() || isInteger();
}

inline bool DataType::isNotNumeric() const
{
    return !isNumeric();
}

#endif  // IBC_DATATYPE_H
