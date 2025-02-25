#pragma once

#include <string_view>
#include <stdexcept>

namespace ast {

enum class TypeSpecifier
{
    INT
};

// Minimal wrapper class to hold both a TypeSpecifier (e.g. “int”)
// and whether it’s signed or unsigned.
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

// Existing operator<< for printing out the TypeSpecifier:
template<typename LogStream>
LogStream& operator<<(LogStream& ls, const TypeSpecifier& type)
{
    const auto TypeToString = [&type] {
        switch (type)
        {
        case TypeSpecifier::INT:
            return "int";
        }
        throw std::runtime_error("Unexpected type specifier");
    };
    return ls << TypeToString();
}

} // namespace ast
