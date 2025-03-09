#include "ast_type_specifier.hpp"

namespace ast {

    bool IsSigned(TypeSpecifier type) {
        switch (type) {
            case TypeSpecifier::INT:
                return true;
            case TypeSpecifier::POINTER:
                return false;
        }
        throw std::runtime_error("Unexpected type specifier");
    }

    int GetTypeSize(TypeSpecifier type) {
        switch (type) {
            case TypeSpecifier::INT:
            case TypeSpecifier::POINTER:
                return 4;
        }
        throw std::runtime_error("Unexpected type specifier");
    }

}
