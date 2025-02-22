#include "ast_parameter_list.hpp"
#include "ast_context.hpp"
#include "ast_identifier.hpp"

namespace ast {

    ParameterList::ParameterList(ParameterDeclarationPtr first) : parameters_() {
        parameters_.push_back(std::move(first));
    }

    void ParameterList::EmitRISC(std::ostream &stream, Context &context, int destReg) const {
        if (!parameters_.empty()) {
            int idx = 0;
            for (const auto& param: parameters_) {
                // TODO types sizes etc
                // TODO correct frame size and stack structure logic
                int offset = -32 + 4 * (static_cast<int>(Size()) - idx);
                context.CurrentFrame().bindings.insert({param->GetIdentifier(), Variable{
                        .offset = offset,
                        .size = 4,
                        .reg = -1
                }});
                // TODO this overlaps the stack with the return address - fix indexing logic
                stream << "sw a" << idx << ", " << offset << "(s0)" << std::endl;
                ++idx;
            }
        }
    }

    void ParameterList::EmitLabelRISC(std::ostream &stream) const {
        stream << "(";
        for (auto it = parameters_.begin(); it != parameters_.end(); ++it) {
            // TODO fix when types are supported
            stream << "int";
            if (it + 1 != parameters_.end()) {
                stream << ", ";
            }
        }
        stream << ")";
    }

    void ParameterList::Print(std::ostream &stream) const {
        // (int x, int y, int z)
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
