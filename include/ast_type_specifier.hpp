#pragma once

#include <string_view>
#include <stdexcept>
#include <vector>
#include <memory>
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
            FLOAT,
            DOUBLE,
            POINTER,
            VOID,
            ENUM,
            STRUCT,
            ARRAY,
        };

        // This is kind of ugly but not too bad
        // Bigger question is do we change all switches to call enum getter, or lose safety
        // Also will mean a lot of throw unsupported cases but that's ok
        // There's going to be a lot of changes for unfolding typedefs as well as aggregate types stuff
        // I think we're on the right track but need to decide what gettype returns, ideally unfolds typedef - can we do that here?
        // maybe we can do away with typedef here entirely - the parser returns the right type already, as long as we can sustain that for structs and enums

        // Expose inline constexpr aliases to preserve current usage.
        inline static constexpr Type INT = Type::INT;
        inline static constexpr Type CHAR = Type::CHAR;
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

        // Complex type constructors
        TypeSpecifier(Type type, TypeSpecifier pointee): type_(POINTER), pointeeType_(std::make_shared<TypeSpecifier>(pointee)) {} // Type rqd to avoid making copy constructor
        TypeSpecifier(TypeSpecifier arrayType, int size): type_(ARRAY), arrayType_(std::make_shared<TypeSpecifier>(arrayType)), arraySize_(size) {}


        ~TypeSpecifier() = default;

        constexpr operator Type() const { return type_; }

        [[nodiscard]] bool IsPointer() const;

        [[nodiscard]] bool IsArray() const;

        [[nodiscard]] bool IsEnum() const;

        [[nodiscard]] bool IsStruct() const;

        bool IsSigned() const;

        int GetTypeSize() const;

        // Getters
        [[nodiscard]] const TypeSpecifier &GetPointeeType() const;

        [[nodiscard]] const TypeSpecifier &GetArrayType() const;

        [[nodiscard]] int GetArraySize() const;

        [[nodiscard]] const std::unordered_map<std::string, int> &GetEnumValues() const;

        [[nodiscard]] const std::string &GetEnumIdentifier() const;

        [[nodiscard]] const std::vector<std::pair<std::string, TypeSpecifierPtr>> &GetStructMembers() const;

        [[nodiscard]] const std::string &GetStructIdentifier() const;

        // todo array and pointer instantiation and use of this class

    private:
        Type type_;

        TypeSpecifierPtr pointeeType_{nullptr};
        TypeSpecifierPtr arrayType_{nullptr};
        int arraySize_{0};
        std::unordered_map<std::string, int> enumValues_{};
        std::string enumIdentifier_{};
        std::vector<std::pair<std::string, TypeSpecifierPtr>> structMembers_{};
        std::string structIdentifier_{};
    };

    // will probably have to store struct and enum names in context, but also here for print if nothing else

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
