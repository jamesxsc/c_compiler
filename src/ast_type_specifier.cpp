#include <cassert>
#include <numeric>
#include <algorithm>
#include "ast_type_specifier.hpp"

namespace ast {

    bool TypeSpecifier::IsSigned() const {
        switch (type_) {
            case TypeSpecifier::INT:
            case TypeSpecifier::FLOAT:
            case TypeSpecifier::DOUBLE:
            case TypeSpecifier::ENUM:
                return true;
            case TypeSpecifier::CHAR: // Implementation defined, but required unsigned for RISCV ABI
            case TypeSpecifier::UNSIGNED:
                return false;
            case TypeSpecifier::POINTER:
            case TypeSpecifier::VOID:
            case TypeSpecifier::STRUCT:
            case TypeSpecifier::ARRAY:
                throw std::runtime_error("Attempted to get sign of non-numeric type");
        }
        throw std::runtime_error("Unexpected type specifier");
    }

    int TypeSpecifier::GetTypeSize() const {
        switch (type_) {
            case TypeSpecifier::CHAR:
                return 1;
            case TypeSpecifier::INT:
            case TypeSpecifier::POINTER:
            case TypeSpecifier::UNSIGNED:
            case TypeSpecifier::FLOAT:
            case TypeSpecifier::Type::ENUM:
                return 4;
            case TypeSpecifier::DOUBLE:
                return 8;
            case TypeSpecifier::Type::VOID:
                return 0;
            case TypeSpecifier::Type::STRUCT:
                assert(structMembers_.has_value() &&
                       "TypeSpecifier::GetTypeSize() called on struct type with unset members");
                return std::accumulate((*structMembers_).begin(), (*structMembers_).end(), 0,
                                       [](int acc, const auto &member) { return acc + member.second.GetTypeSize(); });
            case TypeSpecifier::Type::ARRAY:
                return arrayType_->GetTypeSize() * arraySize_;
        }
        throw std::runtime_error("Unexpected type specifier");
    }

    int TypeSpecifier::GetAlignment() const {
        switch (type_) {
            case Type::CHAR:
                return 1;
            case Type::INT:
            case Type::UNSIGNED:
            case Type::ENUM:
            case Type::FLOAT:
            case Type::POINTER:
                return 4;
            case Type::DOUBLE:
                return 8;
            case Type::VOID:
                break;
            case Type::STRUCT:
                return std::max_element(structMembers_->begin(), structMembers_->end(),
                                        [](const auto &a, const auto &b) {
                                            return a.second.GetAlignment() < b.second.GetAlignment();
                                        })->second.GetAlignment();
            case Type::ARRAY:
                return GetArrayType().GetTypeSize(); // Element alignment
        }
        throw std::runtime_error("TypeSpecifier::GetAlignment() Unexpected type specifier");
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

    void TypeSpecifier::SetMembers(std::vector<std::pair<std::string, TypeSpecifier>> members) {
        assert(IsStruct() && "TypeSpecifier::SetMembers() called on non-struct type");
        structMembers_ = std::move(members);
        // For fast offset lookup
        structMemberOffsetsMap_ = std::make_optional<std::unordered_map<std::string, int>>({});
        int memberOffset = 0;
        for (const auto &member: *structMembers_) {
            structMemberOffsetsMap_->insert({member.first, memberOffset});
            memberOffset += member.second.GetTypeSize();
        }
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

    const std::string &TypeSpecifier::GetEnumIdentifier() const {
        assert(IsEnum() && "TypeSpecifier::GetEnumIdentifier() called on non-enum type");
        return enumIdentifier_;
    }

    const std::vector<std::pair<std::string, TypeSpecifier>> &TypeSpecifier::GetStructMembers() const {
        assert(IsStruct() && "TypeSpecifier::GetStructMembers() called on non-struct type");
        assert(structMembers_.has_value() &&
               "TypeSpecifier::GetStructMembers() called on struct type with unset members");
        return *structMembers_;
    }

    const std::string &TypeSpecifier::GetStructIdentifier() const {
        assert(IsStruct() && "TypeSpecifier::GetStructIdentifier() called on non-struct type");
        return structIdentifier_;
    }

    bool TypeSpecifier::UseStack() const {
        // Conveniently seems that sames rules apply to params and return
        if (!IsStruct() || structMembers_->empty()) return false;
        int nonPadding = static_cast<int>(std::ranges::count_if(*structMembers_, [](const auto &member) {
            return member.first != "#padding";
        }));
        return nonPadding > 2; // 2 is the number of registers available for return
    }

    int TypeSpecifier::GetStructMemberOffset(const std::string &member) const {
        assert(structMemberOffsetsMap_.has_value() &&
               "TypeSpecifier::GetStructMemberOffset() called on struct type with unset member offsets");
        return structMemberOffsetsMap_->at(member);
    }

}
