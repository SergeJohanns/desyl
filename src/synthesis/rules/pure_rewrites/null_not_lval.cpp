#include <null_not_lval.hpp>

#include <generic_continuations/identity_continuation.hpp>
#include <algorithm>
#include <iostream>

namespace desyl
{

    std::vector<Derivation> NullNotLValRule::apply(Goal const &goal) const
    {
        std::vector<Expression> negations;
        for (auto const &array_declaration : goal.spec.precondition.heap.array_declarations)
        {
            for (int i = 0; i < array_declaration.size; ++i)
            {
                auto const &null = BinaryOperatorCall{
                    .type = BinaryOperator::Eq,
                    .lhs = std::make_shared<Expression>(array_declaration.name),
                    .rhs = std::make_shared<Expression>(0),
                };
                auto const &not_null = UnaryOperatorCall{
                    .type = UnaryOperator::Not,
                    .operand = std::make_shared<Expression>(null),
                };
                if (std::find(goal.spec.precondition.proposition.begin(), goal.spec.precondition.proposition.end(), Expression{not_null}) == goal.spec.precondition.proposition.end())
                {
                    negations.push_back(not_null);
                }
            }
        }
        if (negations.empty())
        {
            return {};
        }
        std::vector<Derivation> derivations;
        Goal new_goal(goal);
        for (auto const &negation : negations)
        {
            new_goal.spec.precondition.proposition.push_back(negation);
        }
        derivations.push_back(Derivation{
            .goals = {new_goal},
            .continuation = std::make_unique<IdentityContinuation>(),
        });
        return derivations;
    }
}
