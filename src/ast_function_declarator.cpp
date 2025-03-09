#include "ast_function_declarator.hpp"
#include "ast_parameter_declaration.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void FunctionDeclarator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Store args
        if (parameterList_) {
            parameterList_->EmitRISC(stream, context, destReg);
        }
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
        if (parameterList_) {
            for (auto it = parameterList_->begin(); it != parameterList_->end(); ++it) {
                (*it)->Print(stream);
                if (it + 1 != parameterList_->end()) {
                    stream << ", ";
                }
            }
        }
        stream << ")";
    }

    bool FunctionDeclarator::IsFunction() const {
        return true;
    }

    Function FunctionDeclarator::BuildFunction(TypeSpecifier returnType, Context &context) const {
        if (!parameterList_) {
            return {
                    .parameterSizes = {},
                    .totalSize = 0,
                    .returnType = returnType,
            };
        }

        std::vector<int> parameterSizes;
        int totalSize = 0;
        for (const auto &param: *parameterList_) {
            int size = GetTypeSize(param->GetType(context));
            parameterSizes.push_back(size);
            totalSize += size;
        }
        return {
                .parameterSizes = parameterSizes,
                .totalSize = totalSize,
                .returnType = returnType,
        };
    }

}