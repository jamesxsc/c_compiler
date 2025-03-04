#pragma once

#include <string_view>
#include <stdexcept>

namespace ast {

enum class TypeSpecifier
{
    INT,
    FLOAT,
    DOUBLE
};

class Type {
public:
    explicit Type(TypeSpecifier spec, bool isSigned = true)
        : specifier_(spec), is_signed_(isSigned)
    {}

    bool IsSigned() const { return is_signed_; }
    TypeSpecifier GetSpecifier() const { return specifier_; }

private:
    TypeSpecifier specifier_;
    bool is_signed_;
};

template<typename LogStream>
LogStream& operator<<(LogStream& ls, const TypeSpecifier& type)
{
    switch (type)
    {
        case TypeSpecifier::INT:    ls << "int";    break;
        case TypeSpecifier::FLOAT:  ls << "float";  break;
        case TypeSpecifier::DOUBLE: ls << "double"; break;
    }
    return ls;
}

} // namespace ast
