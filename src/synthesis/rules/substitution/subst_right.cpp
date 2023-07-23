#include <subst_right.hpp>

#include <unification.hpp>
#include <generic_continuations/identity_continuation.hpp>
#include <optional>
#include <algorithm>
#include <iostream>

namespace desyl
{
    Substitutions get_substitutions(Goal const &goal, Vars const &variables)
    {
        Substitutions substitutions;
        for (auto const &clause : goal.spec.postcondition.proposition)
        {
            if (std::holds_alternative<BinaryOperatorCall>(clause))
            {
                auto const &call = std::get<BinaryOperatorCall>(clause);
                if (call.type != BinaryOperator::Eq || !std::holds_alternative<Identifier>(*call.lhs))
                {
                    continue;
                }
                auto const &identifier = std::get<Identifier>(*call.lhs);
                if (std::find(variables.begin(), variables.end(), identifier) == variables.end())
                {
                    continue;
                }
                substitutions[identifier] = *call.rhs;
            }
        }
        return substitutions;
    }

    std::vector<Derivation> SubstRightRule::apply(Goal const &goal) const
    {
        Vars exisentials = goal.variables().existentials();
        auto const &substitutions = get_substitutions(goal, exisentials);
        if (substitutions.size() == 0)
        {
            return {};
        }
        std::vector<Derivation> derivations;
        derivations.push_back(Derivation{
            .goals = std::vector<Goal>{substitute(goal, substitutions)},
            .continuation = std::make_unique<IdentityContinuation>(IdentityContinuation()),
        });
        return derivations;
    }
}
