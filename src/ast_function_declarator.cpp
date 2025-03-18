#include "ast_function_declarator.hpp"
#include "ast_parameter_declaration.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void FunctionDeclarator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Store args; never null
        parameterList_->EmitRISC(stream, context, destReg);
    }

    void FunctionDeclarator::EmitLabelRISC(std::ostream &stream) const {
        stream << ".globl " << GetIdentifier() << std::endl;
        stream << GetIdentifier();
        stream << ":" << std::endl;
    }

    void FunctionDeclarator::Print(std::ostream &stream) const {
        stream << GetIdentifier();
        stream << "(";
        // Print with comma separation
        for (auto it = parameterList_->begin(); it != parameterList_->end(); ++it) {
            (*it)->Print(stream);
            if (it + 1 != parameterList_->end()) {
                stream << ", ";
            }
        }
        stream << ")";
    }

    bool FunctionDeclarator::IsFunction() const {
        return true;
    }

    Function FunctionDeclarator::BuildFunction(TypeSpecifier returnType, Context &context) const {
        if (pointerReturn_)
            returnType = {TypeSpecifier::POINTER, returnType};

        std::vector<int> parameterSizes;
        int totalSize = 0;
        for (const auto &param: *parameterList_) {
            int size = param->GetType(context).GetTypeSize();
            parameterSizes.push_back(size);
            totalSize += size;
        }
        return {
                .parameterSizes = parameterSizes,
                .totalSize = totalSize,
                .returnType = returnType,
        };
    }

    void FunctionDeclarator::SetPointerReturn(int indirectionLevel) {
        pointerReturn_ = true;
        indirectionLevel_ = indirectionLevel;
    }

    bool FunctionDeclarator::GetPointerReturn() const {
        return pointerReturn_;
    }

    void FunctionDeclarator::SetHiddenPointerReturn() const {
        parameterList_->SetHiddenPointerReturn();
    }

}
