#include <pre_inconsistent.hpp>

#include <failure.hpp>
#include <generic_continuations.hpp>
#include <desyl/ast.hpp>
#include <iostream>

namespace desyl
{
    std::vector<Derivation> PreInconsistentRule::apply(Goal const &goal) const
    {
        for (auto const &a : goal.spec.precondition.proposition)
        {
            for (auto const &b : goal.spec.precondition.proposition)
            {
                Expression negated_a = UnaryOperatorCall{
                    .type = UnaryOperator::Not,
                    .operand = std::make_shared<Expression>(a),
                };
                if (negated_a == b)
                {
                    std::cout << "Using PREINCONSISTENT" << std::endl;
                    throw Failure();
                }
            }
        }
        return {};
    }
}
