#include "ast_parameter_list.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    ParameterList::ParameterList(ParameterDeclarationPtr first) : parameters_() {
        parameters_.push_back(std::move(first));
    }

    void ParameterList::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (!parameters_.empty()) {
            int idx = 0;
            for (const auto& param: parameters_) {
                int offset = -20 - 4 * idx;
                context.CurrentFrame().bindings.insert({param->GetIdentifier(), Variable{
                        .offset = offset,
                        .size = 4,
                        .reg = Register::zero,
                        .type = param->GetType(context)
                }});
                stream << "sw a" << idx << ", " << offset << "(s0)" << std::endl;
                ++idx;
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
