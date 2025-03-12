#include "ast_constant.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void IntConstant::EmitRISC(std::ostream &stream, Context &, Register destReg) const {
        stream << "li " << destReg << "," << value_ << std::endl;
    }

    void IntConstant::Print(std::ostream &stream) const {
        stream << value_;
    }

    bool IntConstant::ContainsFunctionCall() const {
        return false;
    }

    std::string IntConstant::GetGlobalIdentifier() const {
        throw std::runtime_error("IntConstant::GetGlobalIdentifier() called on an int constant");
    }

    int IntConstant::GetGlobalValue() const {
        return value_;
    }

    TypeSpecifier IntConstant::GetType(ast::Context &context) const {
        return TypeSpecifier::INT;
    }

    void FloatConstant::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        std::string memoryLabel = context.MakeLabel(".LC");
        Register tempIntReg = context.AllocateTemporary();
        stream << "lui " << tempIntReg << ",%hi(" << memoryLabel << ")" << std::endl;
        stream << "flw " << destReg << ",%lo(" << memoryLabel << ")(" << tempIntReg << ")" << std::endl;
        context.FreeTemporary(tempIntReg);

        // Defer memory to the end
        context.DeferredRISC() << memoryLabel << ":" << std::endl;
        context.DeferredRISC() << ".float " << value_ << std::endl; // todo should we convert to decimal?
        // todo double constants?
    }

    void FloatConstant::Print(std::ostream &stream) const {
        stream << value_;
    }

    bool FloatConstant::ContainsFunctionCall() const {
        return false;
    }

    int FloatConstant::GetGlobalValue() const {
        // We don't need to support implicit casting to int
        throw std::runtime_error("FloatConstant::GetGlobalValue() called on a float constant");
    }

    std::string FloatConstant::GetGlobalIdentifier() const {
        throw std::runtime_error("FloatConstant::GetGlobalIdentifier() called on a float constant");
    }

    TypeSpecifier FloatConstant::GetType(Context &context) const {
        return TypeSpecifier::FLOAT;
    }

} // namespace ast
