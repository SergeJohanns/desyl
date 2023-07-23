#include <true_elision.hpp>

#include <generic_continuations/identity_continuation.hpp>
#include <iostream>

namespace desyl
{
    std::vector<Derivation> TrueElisionRule::apply(Goal const &goal) const
    {
        std::vector<size_t> true_indices;
        for (long long i = goal.spec.postcondition.proposition.size() - 1; i >= 0; --i)
        {
            if (std::holds_alternative<Identifier>(goal.spec.postcondition.proposition[i]) &&
                std::get<Identifier>(goal.spec.postcondition.proposition[i]) == "true")
            {
                true_indices.push_back(i);
            }
        }
        if (true_indices.empty())
        {
            return {};
        }
        std::vector<Derivation> derivations;
        Goal new_goal(goal);
        for (auto i : true_indices)
        {
            new_goal.spec.postcondition.proposition.erase(new_goal.spec.postcondition.proposition.begin() + i);
        }
        derivations.push_back(Derivation{
            .goals = {new_goal},
            .continuation = std::make_unique<IdentityContinuation>(),
        });
        return derivations;
    }
}
