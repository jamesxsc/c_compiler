#pragma once

#include <string_view>
#include <stdexcept>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>

namespace ast {

    // Nice "enum" you have there
    class TypeSpecifier;

    using TypeSpecifierPtr = std::shared_ptr<const TypeSpecifier>;

    class TypeSpecifier {
    public:
        enum class Type {
            INT,
            CHAR,
            UNSIGNED,
            FLOAT,
            DOUBLE,
            POINTER,
            VOID,
            ENUM,
            STRUCT,
            ARRAY,
        };

        // Expose inline constexpr aliases to preserve current usage.
        inline static constexpr Type INT = Type::INT;
        inline static constexpr Type CHAR = Type::CHAR;
        inline static constexpr Type UNSIGNED = Type::UNSIGNED;
        inline static constexpr Type FLOAT = Type::FLOAT;
        inline static constexpr Type DOUBLE = Type::DOUBLE;
        inline static constexpr Type POINTER = Type::POINTER;
        inline static constexpr Type VOID = Type::VOID;
        inline static constexpr Type ENUM = Type::ENUM;
        inline static constexpr Type STRUCT = Type::STRUCT;
        inline static constexpr Type ARRAY = Type::ARRAY;

        TypeSpecifier(Type type) : type_(type) {}

        // Implicit construction to support old TypeSpecifier::TYPE syntax
        TypeSpecifier(int value) : type_(static_cast<Type>(value)) {}

        // Complex type constructors, would probably be better as a factory tbh
        TypeSpecifier(Type type, TypeSpecifier pointee): type_(POINTER), pointeeType_(std::make_shared<TypeSpecifier>(pointee)) {} // Type rqd to avoid overloading copy constructor
        TypeSpecifier(TypeSpecifier arrayType, int size): type_(ARRAY), arrayType_(std::make_shared<TypeSpecifier>(arrayType)), arraySize_(size) {}
        explicit TypeSpecifier(std::string identifier, bool isStruct): type_(isStruct ? STRUCT : ENUM), enumIdentifier_(std::move(identifier)), structIdentifier_(enumIdentifier_) {}

        ~TypeSpecifier() = default;

        constexpr operator Type() const { return type_; }

        [[nodiscard]] bool IsPointer() const;

        [[nodiscard]] bool IsArray() const;

        [[nodiscard]] bool IsEnum() const;

        [[nodiscard]] bool IsStruct() const;

        [[nodiscard]] bool IsSigned() const;

        [[nodiscard]] int GetTypeSize() const;

        [[nodiscard]] int GetAlignment() const;

        void SetMembers(std::vector<std::pair<std::string, TypeSpecifier>> members);

        [[nodiscard]] bool UseStack() const;

        // Getters
        [[nodiscard]] const TypeSpecifier &GetPointeeType() const;

        [[nodiscard]] const TypeSpecifier &GetArrayType() const;

        [[nodiscard]] int GetArraySize() const;

        [[nodiscard]] const std::string &GetEnumIdentifier() const;

        [[nodiscard]] const std::vector<std::pair<std::string, TypeSpecifier>> &GetStructMembers() const;

        [[nodiscard]] const std::string &GetStructIdentifier() const;

    private:
        Type type_;

        TypeSpecifierPtr pointeeType_{nullptr};
        TypeSpecifierPtr arrayType_{nullptr};
        int arraySize_{0};
        std::string enumIdentifier_{};
        std::optional<std::vector<std::pair<std::string, TypeSpecifier>>> structMembers_{};
        std::string structIdentifier_{};
    };

    // This is now recursive
    template<typename LogStream>
    LogStream &operator<<(LogStream &ls, const TypeSpecifier &type)  {
        switch (type) {
            case TypeSpecifier::Type::INT:
                ls << "int";
                break;
            case TypeSpecifier::Type::CHAR:
                ls << "char";
                break;
            case TypeSpecifier::Type::UNSIGNED:
                ls << "unsigned";
                break;
            case TypeSpecifier::Type::FLOAT:
                ls << "float";
                break;
            case TypeSpecifier::Type::DOUBLE:
                ls << "double";
                break;
            case TypeSpecifier::Type::POINTER:
                ls << type.GetPointeeType() << "*";
                break;
            case TypeSpecifier::Type::VOID:
                ls << "void";
                break;
            case TypeSpecifier::Type::ENUM:
                ls << "enum " << type.GetEnumIdentifier();
                break;
                // The below will probably never be called but are included for completeness
            case TypeSpecifier::Type::STRUCT:
                ls << "struct " << type.GetStructIdentifier();
                break;
            case TypeSpecifier::Type::ARRAY:
                ls << type.GetArrayType() << "[" << type.GetArraySize() << "]";
                break;
        }
        return ls;
    }

} // namespace ast
