#include "ast_type_specifier.hpp"

namespace ast {

    bool IsSigned(TypeSpecifier type) {
        switch (type) {
            case TypeSpecifier::INT:
            case TypeSpecifier::FLOAT:
            case TypeSpecifier::DOUBLE:
            case TypeSpecifier::POINTER:
                return true;
            case TypeSpecifier::CHAR: // Implementation defined - this is unsigned to match GCC
                return false;
        }
        throw std::runtime_error("Unexpected type specifier");
    }

    int GetTypeSize(TypeSpecifier type) {
        switch (type) {
            case TypeSpecifier::CHAR:
                return 1;
            case TypeSpecifier::INT:
            case TypeSpecifier::POINTER:
            case TypeSpecifier::FLOAT:
                return 4;
            case TypeSpecifier::DOUBLE:
                return 8;
        }
        throw std::runtime_error("Unexpected type specifier");
    }

}
