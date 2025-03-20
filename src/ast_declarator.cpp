#include "ast_declarator.hpp"

namespace ast {

    void Declarator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (isDirect_)
            return; // Nothing to do! This is part of a function
        // Everything is handled in declaration
    }

    void Declarator::Print(std::ostream &stream) const {
        if (IsPointer())
            stream << "*";
        stream << identifier_;
        if (IsArray()) {
            for (const auto &size: arraySizes_) {
                stream << "[";
                if (size) size->Print(stream);
                stream << "]";
            }
        }
    }

    const std::string &Declarator::GetIdentifier() const {
        return identifier_;
    }

    bool Declarator::IsDirect() const {
        return isDirect_;
    }

    void Declarator::Indirect() {
        isDirect_ = false;
    }

    void Declarator::Direct() {
        isDirect_ = true;
    }

    bool Declarator::IsPointer() const {
        return isPointer_;
    }

    bool Declarator::IsFunction() const {
        return false;
    }

    bool Declarator::IsArray() const {
        return isArray_;
    }

    void Declarator::SetPointerReturn(int) {
        throw std::runtime_error("Declarator::SetPointerReturn() called on a non-function declarator");
    }

    Function Declarator::BuildFunction(TypeSpecifier returnType, Context &context) const {
        std::cerr << "Error: Declarator is not a function" << std::endl;
        exit(1);
    }

    Variable Declarator::BuildArray(TypeSpecifier type, Context &context) const {
        for (const auto &size: arraySizes_) {
            if (!size) // This only occurs for a param (or inferred size)
                throw std::runtime_error("Declarator::BuildArray() called on an array without a size");
            type = {type, size->Evaluate(context)};
        }
        return {
                .size = type.GetTypeSize(),
                .type = type,
        };
    }

    void Declarator::SetPointer(int indirectionLevel) {
        isPointer_ = true;
        indirectionLevel_ = indirectionLevel;
    }

    void Declarator::AddArrayDimension(ConstantExpressionPtr size) {
        isArray_ = true;
        arraySizes_.push_back(std::move(size));
        arrayDimension_++;
    }

}
