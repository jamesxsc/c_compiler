#include <cassert>
#include <numeric>
#include "ast_type_specifier.hpp"

namespace ast {

    bool TypeSpecifier::IsSigned() const {
        switch (GetType()) {
            case TypeSpecifier::INT:
            case TypeSpecifier::FLOAT:
            case TypeSpecifier::DOUBLE:
                return true;
            case TypeSpecifier::CHAR: // Implementation defined, but required unsigned for RISCV ABI
                return false;
            case TypeSpecifier::POINTER:
            case TypeSpecifier::Type::VOID:
            case TypeSpecifier::Type::ENUM:
            case TypeSpecifier::Type::STRUCT:
            case TypeSpecifier::Type::ARRAY:
                throw std::runtime_error("Attempted to get sign of non-numeric type");
        }
        throw std::runtime_error("Unexpected type specifier");
    }

    int TypeSpecifier::GetTypeSize() const {
        switch (GetType()) {
            case TypeSpecifier::CHAR:
                return 1;
            case TypeSpecifier::INT:
            case TypeSpecifier::POINTER:
            case TypeSpecifier::FLOAT:
                return 4;
            case TypeSpecifier::DOUBLE:
                return 8;
            case TypeSpecifier::Type::VOID:
                return 0;
            case TypeSpecifier::Type::ENUM:
                return 4; // Enums are always ints
            case TypeSpecifier::Type::STRUCT:
                return std::accumulate(structMembers_.begin(), structMembers_.end(), 0,
                                       [](int acc, const auto &member) { return acc + member.second->GetTypeSize(); });
            case TypeSpecifier::Type::ARRAY:
                return arrayType_->GetTypeSize() * arraySize_;
        }
        throw std::runtime_error("Unexpected type specifier");
    }

    bool TypeSpecifier::IsPointer() const {
        return type_ == Type::POINTER;
    }

    bool TypeSpecifier::IsArray() const {
        return type_ == Type::ARRAY;
    }

    bool TypeSpecifier::IsEnum() const {
        return type_ == Type::ENUM;
    }

    bool TypeSpecifier::IsStruct() const {
        return type_ == Type::STRUCT;
    }

    [[nodiscard]] TypeSpecifier::Type TypeSpecifier::GetType() const {
        return type_;
    }

    const TypeSpecifier &TypeSpecifier::GetPointeeType() const {
        assert(IsPointer() && "TypeSpecifier::GetPointeeType() called on non-pointer type");
        return *pointeeType_;
    }

    const TypeSpecifier &TypeSpecifier::GetArrayType() const {
        assert(IsArray() && "TypeSpecifier::GetArrayType() called on non-array type");
        return *arrayType_;
    }

    int TypeSpecifier::GetArraySize() const {
        assert(IsArray() && "TypeSpecifier::GetArraySize() called on non-array type");
        return arraySize_;
    }

    const std::unordered_map<std::string, int> &TypeSpecifier::GetEnumValues() const {
        assert(IsEnum() && "TypeSpecifier::GetEnumValues() called on non-enum type");
        return enumValues_;
    }

    const std::string &TypeSpecifier::GetEnumIdentifier() const {
        assert(IsEnum() && "TypeSpecifier::GetEnumIdentifier() called on non-enum type");
        return enumIdentifier_;
    }

    const std::vector<std::pair<std::string, TypeSpecifierPtr>> &TypeSpecifier::GetStructMembers() const {
        assert(IsStruct() && "TypeSpecifier::GetStructMembers() called on non-struct type");
        return structMembers_;
    }

    const std::string &TypeSpecifier::GetStructIdentifier() const {
        assert(IsStruct() && "TypeSpecifier::GetStructIdentifier() called on non-struct type");
        return structIdentifier_;
    }

}
