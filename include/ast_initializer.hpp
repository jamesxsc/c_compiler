#pragma once

#include "ast_node.hpp"

namespace ast {

    class Initializer : public Node {
        // TODO impl, basically just takes an assignment expression
        // TODO add initializer list
    };

    using InitializerPtr = std::unique_ptr<const Initializer>;

}
