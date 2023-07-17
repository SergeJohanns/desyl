#include <pure_frame.hpp>

#include <generic_continuations/identity_continuation.hpp>
#include <optional>
#include <iostream>

namespace desyl
{
    std::vector<Derivation> PureFrameRule::apply(Goal const &goal) const
    {
        std::vector<Derivation> derivations;
        for (size_t i = 0; i < goal.spec.precondition.proposition.size(); ++i)
        {
            for (size_t j = 0; j < goal.spec.postcondition.proposition.size(); ++j)
            {
                if (goal.spec.precondition.proposition[i] == goal.spec.postcondition.proposition[j])
                {
                    std::cout << "Using PUREFRAME" << std::endl;
                    auto new_goal(goal);
                    new_goal.spec.postcondition.proposition.erase(new_goal.spec.postcondition.proposition.begin() + j);
                    derivations.push_back(Derivation{
                        .goals = {new_goal},
                        .continuation = std::make_unique<IdentityContinuation>(),
                    });
                    return derivations; // Changing the size of the postcondition vector invalidates the iterators.
                };
            }
        }
        return derivations;
    }
}
