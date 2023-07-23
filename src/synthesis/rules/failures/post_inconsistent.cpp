#include <post_inconsistent.hpp>

#include <failure.hpp>
#include <generic_continuations/identity_continuation.hpp>
#include <desyl/ast.hpp>
#include <iostream>

namespace desyl
{
    std::vector<Derivation> PostInconsistentRule::apply(Goal const &goal) const
    {
        for (auto const &a : goal.spec.precondition.proposition)
        {
            Expression negated_a = UnaryOperatorCall{
                .type = UnaryOperator::Not,
                .operand = std::make_shared<Expression>(a),
            };
            for (auto const &b : goal.spec.precondition.proposition)
            {
                if (negated_a == b)
                {
                    throw Failure();
                }
            }
            for (auto const &b : goal.spec.postcondition.proposition)
            {
                if (negated_a == b)
                {
                    throw Failure();
                }
            }
        }
        for (auto const &a : goal.spec.postcondition.proposition)
        {
            Expression negated_a = UnaryOperatorCall{
                .type = UnaryOperator::Not,
                .operand = std::make_shared<Expression>(a),
            };
            for (auto const &b : goal.spec.precondition.proposition)
            {
                if (negated_a == b)
                {
                    throw Failure();
                }
            }
            for (auto const &b : goal.spec.postcondition.proposition)
            {
                if (negated_a == b)
                {
                    throw Failure();
                }
            }
        }
        return {};
    }
}
