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

}
