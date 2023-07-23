#include <close.hpp>

#include <predicates.hpp>
#include <substitution/substitution.hpp>
#include <generic_continuations/identity_continuation.hpp>
#include <sstream>
#include <algorithm>
#include <ranges>
#include <iostream>

namespace desyl
{
    std::vector<Derivation> CloseRule::apply(Goal const &goal) const
    {
        std::vector<Derivation> derivations;
        Vars const &environment = goal.environment;
        VariableClassification old_classification = goal.variables().classification;
        for (size_t i = 0; i < goal.spec.postcondition.heap.predicate_calls.size(); ++i)
        {
            auto const &predicate_call = goal.spec.postcondition.heap.predicate_calls[i];
            const bool predicate_exists = goal.predicates.find(predicate_call.name) != goal.predicates.end();
            if (predicate_call.label >= MAX_UNFOLDS || !predicate_exists)
            {
                continue;
            }
            auto const &predicate = goal.predicates.at(predicate_call.name);
            if (predicate.args.size() != predicate_call.args.size())
            {
                continue;
            }
            Substitutions substitution;
            for (size_t i = 0; i < predicate.args.size(); i++)
            {
                substitution[predicate.args[i].name] = predicate_call.args[i];
            }
            for (auto const &clause : predicate.clauses)
            {
                FunctionSpecification new_spec = goal.spec;

                new_spec.postcondition.proposition.push_back(substitute(clause.condition, substitution));
                Proposition consequent = substitute(clause.assertion.proposition, substitution);
                new_spec.postcondition.proposition.insert(new_spec.postcondition.proposition.end(), consequent.begin(), consequent.end());

                Heap heap = substitute(clause.assertion.heap, substitution);
                new_spec.precondition.heap.pointer_declarations.insert(new_spec.precondition.heap.pointer_declarations.end(), heap.pointer_declarations.begin(), heap.pointer_declarations.end());
                new_spec.precondition.heap.array_declarations.insert(new_spec.precondition.heap.array_declarations.end(), heap.array_declarations.begin(), heap.array_declarations.end());
                new_spec.precondition.heap.predicate_calls.insert(new_spec.precondition.heap.predicate_calls.end(), heap.predicate_calls.begin(), heap.predicate_calls.end());
                for (auto &predicate_call : new_spec.precondition.heap.predicate_calls)
                {
                    predicate_call.label++;
                }

                new_spec.postcondition.heap.predicate_calls.erase(new_spec.postcondition.heap.predicate_calls.begin() + i);
                Goal new_goal(std::move(new_spec), goal.functions, goal.predicates);
                VariableClassification new_classification = new_goal.variables().classification;
                Vars difference;

                // Make sure that the old classification is a subset of the new classification
                std::set_difference(new_classification.ghosts.begin(), new_classification.ghosts.end(),
                                    old_classification.existentials.begin(), old_classification.existentials.end(),
                                    std::inserter(difference, difference.begin()));
                std::set_difference(difference.begin(), difference.end(),
                                    environment.begin(), environment.end(),
                                    std::inserter(new_classification.ghosts, new_classification.ghosts.begin()));
                std::set_difference(new_classification.existentials.begin(), new_classification.existentials.end(),
                                    old_classification.ghosts.begin(), old_classification.ghosts.end(),
                                    std::inserter(difference, difference.begin()));
                std::set_difference(difference.begin(), difference.end(),
                                    environment.begin(), environment.end(),
                                    std::inserter(new_classification.existentials, new_classification.existentials.begin()));
                derivations.push_back(Derivation{
                    .goals = {new_goal},
                    .continuation = std::make_unique<IdentityContinuation>(),
                });
            }
        }
        return derivations;
    }
}
