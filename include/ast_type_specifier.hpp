#pragma once

#include <string_view>
#include <stdexcept>

namespace ast {

    enum class TypeSpecifier {
        INT,
        POINTER,
    };

    bool IsSigned(TypeSpecifier type);

    // Existing operator<< for printing out the TypeSpecifier:
    template<typename LogStream>
    LogStream &operator<<(LogStream &ls, const TypeSpecifier &type) {
        const auto TypeToString = [&type] {
            switch (type) {
                case TypeSpecifier::INT:
                    return "int";
                case TypeSpecifier::POINTER:
                    break;
            }
            throw std::runtime_error("Unexpected type specifier");
        };
        return ls << TypeToString();
    }

} // namespace ast
