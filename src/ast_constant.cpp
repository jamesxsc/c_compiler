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

    void FloatConstant::EmitRISC(std::ostream &stream, Context &, Register destReg) const {
        // todo check this logic
        int bits = *reinterpret_cast<const int *>(&value_);
        stream << "li " << destReg << "," << bits << std::endl;
    }

    void FloatConstant::Print(std::ostream &stream) const {
        stream << value_;
    }

    bool FloatConstant::ContainsFunctionCall() const {
        return false;
    }

    int FloatConstant::GetGlobalValue() const {
        // todo should this be assignable to a global int without a cast?
        throw std::runtime_error("FloatConstant::GetGlobalValue() called on a float constant");
    }

    std::string FloatConstant::GetGlobalIdentifier() const {
        throw std::runtime_error("FloatConstant::GetGlobalIdentifier() called on a float constant");
    }

    TypeSpecifier FloatConstant::GetType(Context &context) const {
        return TypeSpecifier::FLOAT;
    }

} // namespace ast
