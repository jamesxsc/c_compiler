#include "ast_parameter_list.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    ParameterList::ParameterList(ParameterDeclarationPtr first) : parameters_() {
        parameters_.push_back(std::move(first));
    }

    void ParameterList::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (!parameters_.empty()) {
            int iidx = 0, fidx = 0;
            for (const auto &param: parameters_) {
                TypeSpecifier type = param->GetType(context);
                Variable var = context.CurrentFrame().bindings.Insert(param->GetIdentifier(), Variable{
                        .size = type.GetTypeSize(),
                        .type = type
                });
                switch (type) {
                    case TypeSpecifier::FLOAT:
                    case TypeSpecifier::DOUBLE:
                        // TODO: ABI: 8-byte alignment?
                        stream << (type == TypeSpecifier::FLOAT ? "fsw " : "fsd ")
                               << "fa" << fidx << "," << var.offset << "(s0)" << std::endl;
                        ++fidx;
                        break;
                    case TypeSpecifier::POINTER:
                    case TypeSpecifier::INT:
                        stream << "sw a" << iidx << "," << var.offset << "(s0)" << std::endl;
                        ++iidx;
                        break;
                    case TypeSpecifier::CHAR:
                        stream << "sb a" << iidx << "," << var.offset << "(s0)" << std::endl;
                        ++iidx;
                        break;
                    case TypeSpecifier::VOID:
                    case TypeSpecifier::ENUM:
                    case TypeSpecifier::STRUCT:
                    case TypeSpecifier::ARRAY:
                        throw std::runtime_error(
                                "ParameterList::EmitRISC() called on an unsupported array type");
                        // todo handle these
                }
            }
        }
    }

    void ParameterList::Print(std::ostream &stream) const {
        if (!parameters_.empty()) {
            stream << "(";
            for (auto it = parameters_.begin(); it != parameters_.end(); ++it) {
                (*it)->Print(stream);
                if (it + 1 != parameters_.end()) {
                    stream << ", ";
                }
            }
            stream << ")";
        }
    }

    void ParameterList::PushBack(ParameterDeclarationPtr item) {
        parameters_.push_back(std::move(item));
    }

    size_t ParameterList::Size() const {
        return parameters_.size();
    }

    std::vector<ParameterDeclarationPtr>::const_iterator ParameterList::begin() const {
        return parameters_.begin();
    }

    std::vector<ParameterDeclarationPtr>::const_iterator ParameterList::end() const {
        return parameters_.end();
    }

}
