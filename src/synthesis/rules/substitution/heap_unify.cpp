#include <heap_unify.hpp>

#include <generic_continuations/generic_continuations.hpp>
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
                auto const &unified_base = unify(*postcondition.base, *precondition.base, existentials);
                if (!unified_base)
                {
                    continue;
                }
                auto const &unified_value = unify(*postcondition.value, *precondition.value, existentials);
                if (!unified_value || subtitutions_conflict(*unified_base, *unified_value))
                {
                    continue;
                }
                Substitutions unified;
                unified.insert(unified_base->begin(), unified_base->end());
                unified.insert(unified_value->begin(), unified_value->end());
                // If both are trivially equal, then we don't need to add the substitution
                if (unified.size() != 0)
                {
                    substitutions.push_back(unified);
                }
            }
        }
    }

    void add_predicate_substitutions(Goal const &, std::vector<Substitutions> &)
    {
        // TODO
    }

    std::vector<Derivation> HeapUnifyRule::apply(Goal const &goal) const
    {
        std::vector<Substitutions> substitutions;
        add_pointer_substitutions(goal, substitutions);
        add_predicate_substitutions(goal, substitutions);
        if (substitutions.size() > 0)
        {
            std::cout << "Using HEAPUNIFY" << std::endl;
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
