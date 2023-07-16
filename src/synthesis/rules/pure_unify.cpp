#include <pure_unify.hpp>

#include <generic_continuations.hpp>
#include <unification.hpp>
#include <iostream>

namespace desyl
{
    void add_substitutions(Goal const &goal, std::vector<Substitutions> &substitutions)
    {
        auto const &existentials = goal.variables().existentials();
        for (auto const &precondition : goal.spec.precondition.proposition)
        {
            for (auto const &postcondition : goal.spec.postcondition.proposition)
            {
                auto substitution = unify(postcondition, precondition, existentials);
                if (substitution.has_value() && substitution.value().size() > 0)
                {
                    substitutions.push_back(substitution.value());
                }
            }
        }
    }

    std::vector<Derivation> PureUnifyRule::apply(Goal const &goal) const
    {
        std::vector<Substitutions> substitutions;
        add_substitutions(goal, substitutions);
        if (substitutions.size() > 0)
        {
            std::cout << "Using PUREUNIFY" << std::endl;
        }
        std::vector<Derivation> derivations;
        for (auto const &substitution : substitutions)
        {
            derivations.push_back(Derivation{
                .goals = std::vector<Goal>{substitute(goal, substitution)},
                .continuation = std::make_unique<IdentityContinuation>(IdentityContinuation()),
            });
        }
        return derivations;
    }
}
