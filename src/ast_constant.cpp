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

    void CharConstant::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        stream << "li " << destReg << "," << value_ << std::endl;
    }

    void CharConstant::Print(std::ostream &stream) const {
        char c = static_cast<char>(value_);
        switch (c) {
            case '\n':
                stream << "'\\n'";
                break;
            case '\t':
                stream << "'\\t'";
                break;
            case '\r':
                stream << "'\\r'";
                break;
            case '\v':
                stream << "'\\v'";
                break;
            case '\b':
                stream << "'\\b'";
                break;
            case '\f':
                stream << "'\\f'";
                break;
            case '\a':
                stream << "'\\a'";
                break;
            case '\\':
                stream << "'\\\\'";
                break;
            case '\'':
                stream << "'\\''";
                break;
            case '\"':
                stream << "'\\\"'";
                break;
            default:
                if (c >= 32 && c <= 126) {
                    stream << "'" << c << "'";
                } else {
                    stream << "'\\x" << std::hex << static_cast<int>(c) << "'";
                }
        }
    }

    bool CharConstant::ContainsFunctionCall() const {
        return false;
    }

    int CharConstant::GetGlobalValue() const {
        return value_;
    }

    std::string CharConstant::GetGlobalIdentifier() const {
        throw std::runtime_error("CharConstant::GetGlobalIdentifier() called on an int constant");
    }

    TypeSpecifier CharConstant::GetType(Context &context) const {
        return TypeSpecifier::CHAR;
    }

    void FloatConstant::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        std::string memoryLabel = context.MakeLabel(".LC");
        Register tempIntReg = context.AllocateTemporary();
        stream << "lui " << tempIntReg << ",%hi(" << memoryLabel << ")" << std::endl;
        stream << (doublePrecision_ ? "fld " : "flw ")
               << destReg << ",%lo(" << memoryLabel << ")(" << tempIntReg << ")" << std::endl;
        context.FreeTemporary(tempIntReg);

        // Defer memory to the end
        context.DeferredRISC() << memoryLabel << ":" << std::endl;
        context.DeferredRISC() << (doublePrecision_ ? ".double " : ".float ")
                               << value_ << std::endl; // todo should we convert to decimal?
    }

    void FloatConstant::Print(std::ostream &stream) const {
        stream << value_ << (doublePrecision_ ? "" : "f");
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


    void StringConstant::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Place in stack and load address into destReg
        std::string memoryLabel = context.MakeLabel(".LC");
        context.DeferredRISC() << ".section .rodata" << std::endl;
        context.DeferredRISC() << ".align 2" << std::endl;
        context.DeferredRISC() << memoryLabel << ":" << std::endl;
        context.DeferredRISC() << ".string \"" << value_ << '\"' << std::endl;

        stream << "lui " << destReg << ",%hi(" << memoryLabel << ")" << std::endl;
        stream << "addi " << destReg << "," << destReg << ",%lo(" << memoryLabel << ")" << std::endl;
    }

    void StringConstant::Print(std::ostream &stream) const {
        stream << '\"' << value_ << '\"';
    }

    bool StringConstant::ContainsFunctionCall() const {
        return false;
    }

    int StringConstant::GetGlobalValue() const {
        throw std::runtime_error("StringConstant::GetGlobalValue() called on a string constant");
    }

    std::string StringConstant::GetGlobalIdentifier() const {
        throw std::runtime_error("StringConstant::GetGlobalIdentifier() called on a string constant");
    }

    TypeSpecifier StringConstant::GetType(Context &context) const {
        return {TypeSpecifier(TypeSpecifier::CHAR), static_cast<int>(value_.length())};
    }
} // namespace ast
