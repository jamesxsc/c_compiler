#pragma once

#include "ast_initializer.hpp"

namespace ast {

    class InitializerList: public Initializer {
    public:
        InitializerList(InitializerPtr first, InitializerPtr second): Initializer(nullptr), initializers_() {
            initializers_.push_back(std::move(first));
            initializers_.push_back(std::move(second));
        }

        ~InitializerList() override;

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        bool IsList() const override;

        void AddInitializer(InitializerPtr initializer) override;

        [[nodiscard]] std::vector<InitializerPtr>::const_iterator begin() const;

        [[nodiscard]] std::vector<InitializerPtr>::const_iterator end() const;
    private:
        std::vector<InitializerPtr> initializers_;
    };

}
