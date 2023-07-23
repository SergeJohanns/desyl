#include <heap_unify.hpp>

#include <generic_continuations/identity_continuation.hpp>
#include <unification.hpp>
#include <overload.hpp>
#include <variant>
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace desyl
{
    void add_pointer_substitutions(Goal const &goal, std::vector<Substitutions> &substitutions)
    {
        auto const &existentials = goal.variables().existentials();
        for (auto const &precondition : goal.spec.precondition.heap.pointer_declarations)
        {
            for (auto const &postcondition : goal.spec.postcondition.heap.pointer_declarations)
            {
                auto const &unified = unify(postcondition, precondition, existentials);
                // If both are trivially equal, then we don't need to add the substitution
                if (unified.has_value() && unified.value().size() != 0)
                {
                    substitutions.push_back(unified.value());
                }
            }
        }
    }

    void add_predicate_substitutions(Goal const &goal, std::vector<Substitutions> &substitutions)
    {
        auto const &existentials = goal.variables().existentials();
        for (auto const &precondition : goal.spec.precondition.heap.predicate_calls)
        {
            for (auto const &postcondition : goal.spec.postcondition.heap.predicate_calls)
            {
                auto const &unified = unify(postcondition, precondition, existentials);
                // If both are trivially equal, then we don't need to add the substitution
                if (unified.has_value() && unified.value().size() != 0)
                {
                    substitutions.push_back(unified.value());
                }
            }
        }
    }

    std::vector<Derivation> HeapUnifyRule::apply(Goal const &goal) const
    {
        std::vector<Substitutions> substitutions;
        add_pointer_substitutions(goal, substitutions);
        add_predicate_substitutions(goal, substitutions);
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
