#include <reflexivity.hpp>

#include <generic_continuations/identity_continuation.hpp>
#include <iostream>

namespace desyl
{
    std::vector<Derivation> ReflexiveRule::apply(Goal const &goal) const
    {
        std::vector<Derivation> derivations;
        auto new_goal(goal); // Can't be copied in the switch statement because of cross-initialization.
        for (size_t i = 0; i < goal.spec.postcondition.proposition.size(); ++i)
        {
            if (!std::holds_alternative<BinaryOperatorCall>(goal.spec.postcondition.proposition[i]))
            {
                continue;
            }
            auto const &call = std::get<BinaryOperatorCall>(goal.spec.postcondition.proposition[i]);
            if (*call.lhs != *call.rhs)
            {
                continue;
            }
            switch (call.type)
            {
            case BinaryOperator::Eq:
            case BinaryOperator::Leq:
            case BinaryOperator::Geq:
                new_goal.spec.postcondition.proposition.erase(new_goal.spec.postcondition.proposition.begin() + i);
                derivations.push_back(Derivation{
                    .goals = {new_goal},
                    .continuation = std::make_unique<IdentityContinuation>(),
                });
                return derivations; // Changing the size of the postcondition vector invalidates the iterators.
            default:
                break;
            }
        }
        return derivations;
    }
}
