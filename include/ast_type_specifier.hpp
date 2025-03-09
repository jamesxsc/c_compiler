#pragma once

#include <string_view>
#include <stdexcept>

namespace ast {

    enum class TypeSpecifier {
        INT,
        FLOAT,
        DOUBLE,
        POINTER,
    };

    bool IsSigned(TypeSpecifier type);

    int GetTypeSize(TypeSpecifier type);

    template<typename LogStream>
    LogStream &operator<<(LogStream &ls, const TypeSpecifier &type) {
        const auto TypeToString = [&type] {
            switch (type) {
                case TypeSpecifier::INT:
                    return "int";
                case TypeSpecifier::FLOAT:
                    return "float";
                case TypeSpecifier::DOUBLE:
                    return "double";
                case TypeSpecifier::POINTER:
                    break;
            }
            throw std::runtime_error("Unexpected type specifier");
        };
        return ls << TypeToString();
    }

} // namespace ast
