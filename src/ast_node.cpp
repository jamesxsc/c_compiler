#include "ast_node.hpp"

namespace ast {

    void NodeList::PushBack(NodePtr item) {
        nodes_.push_back(std::move(item));
    }

    void NodeList::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        for (const auto &node: nodes_) {
            if (node == nullptr) {
                continue;
            }
            node->EmitRISC(stream, context, destReg);
        }
    }

    void NodeList::Print(std::ostream &stream) const {
        for (const auto &node: nodes_) {
            if (node == nullptr) {
                continue;
            }
            node->Print(stream);
        }
    }

    std::vector<NodePtr>::const_iterator NodeList::begin() const {
        return nodes_.begin();
    }

    std::vector<NodePtr>::const_iterator NodeList::end() const {
        return nodes_.end();
    }

    size_t NodeList::Size() const {
        return nodes_.size();
    }

}
