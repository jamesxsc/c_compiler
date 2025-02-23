#include "ast_assignment_expression.hpp"
#include "ast_expression.hpp"

namespace ast {


    void AssignmentExpression::EmitRISC(std::ostream &stream, Context &context, int destReg) const {
        // TODO implement assignment operators
        if (op_ == AssignmentOperator::ConditionalPromote) {
            conditional_->EmitRISC(stream, context, destReg);
        } else {
            std::cerr << "Assignment operator not implemented" << std::endl;
        }
    }

    // TODO print correct operator - update enum with string representation?
    void AssignmentExpression::Print(std::ostream &stream) const {
        if (op_ == AssignmentOperator::ConditionalPromote) {
            conditional_->Print(stream);
        } else {
            unary_->Print(stream);
            stream << " = ";
            assignment_->Print(stream);
        }
    }

    AssignmentExpression::AssignmentExpression(ConditionalExpressionPtr conditional) : op_(AssignmentOperator::ConditionalPromote), conditional_(std::move(conditional)), unary_(
            nullptr), assignment_(nullptr) {}

    AssignmentExpression::AssignmentExpression(UnaryExpressionPtr unary, AssignmentOperator op,
                                               AssignmentExpressionPtr assignment) : op_(op), conditional_(nullptr), unary_(std::move(unary)), assignment_(std::move(assignment)) {}

}
