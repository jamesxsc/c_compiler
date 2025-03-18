#include "ast_parameter_list.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    ParameterList::ParameterList(ParameterDeclarationPtr first) : parameters_() {
        if (first)
            parameters_.push_back(std::move(first));
    }

    void ParameterList::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (parameters_.empty() && !hiddenPointerReturn_) {
            return;
        }

        // TODO handle passed on stack

        int iidx = 0, fidx = 0;
        if (hiddenPointerReturn_) { // Always in first register
            Variable var = context.CurrentFrame().bindings.Insert("#hiddenpointer", Variable{
                    .size = 4,
                    .type = TypeSpecifier::POINTER // Note that the pointee type is not stored
            });
            stream << "sw a" << iidx << "," << var.offset << "(s0)" << std::endl;
            ++iidx;
        }
        for (const auto &param: parameters_) {
            TypeSpecifier type = param->GetType(context);
            Variable var = context.CurrentFrame().bindings.Insert(param->GetIdentifier(), Variable{
                    .size = type.GetTypeSize(),
                    .type = type
            });
            switch (type) {
                case TypeSpecifier::FLOAT:
                case TypeSpecifier::DOUBLE:
                    stream << (type == TypeSpecifier::FLOAT ? "fsw " : "fsd ")
                           << "fa" << fidx << "," << var.offset << "(s0)" << std::endl;
                    ++fidx;
                    break;
                case TypeSpecifier::POINTER:
                case TypeSpecifier::INT:
                case TypeSpecifier::ENUM:
                case TypeSpecifier::UNSIGNED:
                    stream << "sw a" << iidx << "," << var.offset << "(s0)" << std::endl;
                    ++iidx;
                    break;
                case TypeSpecifier::CHAR:
                    stream << "sb a" << iidx << "," << var.offset << "(s0)" << std::endl;
                    ++iidx;
                    break;
                case TypeSpecifier::STRUCT: {
                    int memberOffset = 0;
                    for (const auto& member : type.GetStructMembers()) {
                        if (member.first == "#padding") {
                            memberOffset += member.second.GetTypeSize();
                            continue;
                        }
                        switch (member.second) {
                            case TypeSpecifier::Type::INT:
                            case TypeSpecifier::Type::UNSIGNED:
                            case TypeSpecifier::Type::POINTER:
                            case TypeSpecifier::Type::ARRAY:
                            case TypeSpecifier::Type::ENUM:
                                stream << "sw a" << iidx << "," << var.offset + memberOffset << "(s0)" << std::endl;
                                ++iidx;
                                break;
                            case TypeSpecifier::Type::CHAR:
                                stream << "sb a" << iidx << "," << var.offset + memberOffset << "(s0)" << std::endl;
                                ++iidx;
                                break;
                            case TypeSpecifier::Type::FLOAT:
                            case TypeSpecifier::Type::DOUBLE:
                                stream << (member.second == TypeSpecifier::FLOAT ? "fsw " : "fsd ")
                                       << "fa" << fidx << "," << var.offset + memberOffset << "(s0)" << std::endl;
                                ++fidx;
                                break;
                            case TypeSpecifier::Type::STRUCT: // todo Handle nested structs
                            case TypeSpecifier::Type::VOID:
                                throw std::runtime_error(
                                        "ParameterList::EmitRISC() called on an unsupported struct member type");
                        }
                        memberOffset += member.second.GetTypeSize();
                    }
                    break;
                }
                case TypeSpecifier::VOID:
                case TypeSpecifier::ARRAY:
                    throw std::runtime_error(
                            "ParameterList::EmitRISC() called on an unsupported array type");
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

    void ParameterList::SetHiddenPointerReturn() const {
        hiddenPointerReturn_ = true;
    }

}
