#pragma once

#include "ast_node.hpp"

namespace ast {

    class InitDeclarator : public Node {

    private:
        DeclaratorPtr declarator_;
        InitializerPtr initializer_;
    };

    using InitDeclaratorPtr = std::unique_ptr<const InitDeclarator>;

}
