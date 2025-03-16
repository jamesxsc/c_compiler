#pragma once

#include "ast_node.hpp"
#include "ast_enumerator_list.hpp"

namespace ast {

    class EnumSpecifier : public Node {
    public:
        explicit EnumSpecifier(std::string identifier) : identifier_(std::move(identifier)), enumerators_(nullptr),
                                                         enumeratorsValuesMap_() {};

        explicit EnumSpecifier(EnumeratorListPtr enumerators)
                : identifier_(std::nullopt), enumerators_(std::move(enumerators)) {};

        EnumSpecifier(std::string identifier, EnumeratorListPtr enumerators)
                : identifier_(std::move(identifier)), enumerators_(std::move(enumerators)) {};

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] bool HasIdentifier() const;

        [[nodiscard]] const std::string &GetIdentifier() const;

        [[nodiscard]] bool HasEnumerators() const;

        void SetGlobal();

    private:
        std::optional<std::string> identifier_;
        EnumeratorListPtr enumerators_;
        mutable std::map<std::string, int> enumeratorsValuesMap_{};
        bool isGlobal_{false};
    };
    using EnumSpecifierPtr = std::unique_ptr<EnumSpecifier>;

}
