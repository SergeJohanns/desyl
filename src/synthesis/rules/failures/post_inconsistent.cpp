#include <post_inconsistent.hpp>

#include <failure.hpp>
#include <generic_continuations/generic_continuations.hpp>
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
                    std::cout << "Using POSTINCONSISTENT" << std::endl;
                    throw Failure();
                }
            }
            for (auto const &b : goal.spec.postcondition.proposition)
            {
                if (negated_a == b)
                {
                    std::cout << "Using POSTINCONSISTENT" << std::endl;
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
            for (auto const &b : goal.spec.postcondition.proposition)
            {
                if (negated_a == b)
                {
                    std::cout << "Using POSTINCONSISTENT" << std::endl;
                    throw Failure();
                }
            }
        }
        return {};
    }
}
