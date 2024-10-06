#include <abduce_call.hpp>

#include <substitution/unification.hpp>
#include <call.hpp>
#include <sstream>
#include <algorithm>

namespace desyl
{
    Program AbduceCallContinuation::join(std::vector<Program> const &results) const
    {
        std::ostringstream stream;
        stream << results[0];
        stream << results[1];
        Program program("");
        return program.add_lines(stream.str());
    }

    /// @brief Get all unifications for all of the predicates of the function's precondition, and corresponding predicates in the goal's precondition
    std::vector<Substitutions> unify_predicates(std::vector<PredicateCall> const &from_function, std::vector<PredicateCall> const &from_precondition, Vars const &variables)
    {
        auto unifications = unify_all_clauses(from_function, from_precondition, variables, unify);
        std::vector<Substitutions> result;
        for (auto unification : unifications)
        {
            if (predicate_label_higher(from_function, from_precondition, unification.term_index_mapping))
            {
                result.push_back(unification.substitutions);
            }
        }
        return result;
    }

    std::vector<size_t> find_pointer_equivalents(std::vector<PointerDeclaration> const &substituted, std::vector<PointerDeclaration> const &from_precondition)
    {
        std::vector<size_t> result;
        for (auto const &pointer : substituted)
        {
            // We don't need to exclude indices we checked for earlier elements, because the pointer bases are guaranteed to be unique
            size_t i;
            for (i = 0; i < from_precondition.size(); i++)
            {
                if (pointer.base == from_precondition[i].base)
                {
                    result.push_back(i);
                    break;
                }
            }
            if (i == from_precondition.size())
            {
                return {}; // If we can't find a match for a pointer, the substitution is invalid
            }
        }
        return result;
    }

    std::vector<size_t> find_array_equivalents(std::vector<ArrayDeclaration> const &substituted, std::vector<ArrayDeclaration> const &from_precondition)
    {
        std::vector<size_t> result;
        for (auto const &array : substituted)
        {
            // We don't need to exclude indices we checked for earlier elements, because the array bases are guaranteed to be unique
            size_t i;
            for (i = 0; i < from_precondition.size(); i++)
            {
                if (array == from_precondition[i])
                {
                    result.push_back(i);
                    break;
                }
            }
            if (i == from_precondition.size())
            {
                return {}; // If we can't find a match for an array, the substitution is invalid
            }
        }
        return result;
    }

    std::vector<Derivation> AbduceCallRule::apply(Goal const &goal) const
    {
        std::vector<Derivation> derivations;
        for (auto const &[name, spec] : goal.functions)
        {
            // TODO: Support recursive calls to functions with nontrivial pure preconditions
            if (!spec.precondition.proposition.empty())
            {
                continue;
            }

            Vars variables;
            // Not just program variables but all variables in the precondition can be substituted
            vars(spec.precondition, variables);
            auto substitutions = unify_predicates(spec.precondition.heap.predicate_calls, goal.spec.precondition.heap.predicate_calls, variables);
            for (auto const &substitution : substitutions)
            {
                Heap F_prime(spec.precondition.heap);
                F_prime.predicate_calls = {}; // Predicates have been unified separately and are not changed in the setup to the recursive call
                substitute(F_prime, substitution);
                auto pointer_equivalents = find_pointer_equivalents(F_prime.pointer_declarations, goal.spec.precondition.heap.pointer_declarations);
                auto array_equivalents = find_array_equivalents(F_prime.array_declarations, goal.spec.precondition.heap.array_declarations);
                bool incomplete_equivalents = pointer_equivalents.size() != F_prime.pointer_declarations.size() || array_equivalents.size() != F_prime.array_declarations.size();
                if ((pointer_equivalents.empty() && array_equivalents.empty()) || incomplete_equivalents)
                {
                    continue; // If we can't find a match for a pointer or array, or if the non-predicate part is completely empty (violates well-foundedness requirement) the substitution is invalid
                }

                FunctionSpecification abduce_goal_spec;
                FunctionSpecification call_goal_spec(goal.spec);
                for (size_t i = 0; i < F_prime.pointer_declarations.size(); i++)
                {
                    abduce_goal_spec.precondition.heap.pointer_declarations.push_back(goal.spec.precondition.heap.pointer_declarations[pointer_equivalents[i]]);
                    abduce_goal_spec.postcondition.heap.pointer_declarations.push_back(F_prime.pointer_declarations[i]);
                    call_goal_spec.precondition.heap.pointer_declarations[pointer_equivalents[i]] = F_prime.pointer_declarations[i];
                }
                for (size_t i = 0; i < F_prime.array_declarations.size(); i++)
                {
                    abduce_goal_spec.precondition.heap.array_declarations.push_back(goal.spec.precondition.heap.array_declarations[array_equivalents[i]]);
                    abduce_goal_spec.postcondition.heap.array_declarations.push_back(F_prime.array_declarations[i]);
                    call_goal_spec.precondition.heap.array_declarations[array_equivalents[i]] = F_prime.array_declarations[i];
                }
                Goal call_goal = goal.with_spec(std::move(call_goal_spec));
                Goal abduce_goal = goal.with_spec(std::move(abduce_goal_spec));

                derivations.push_back(Derivation{
                    .goals = std::vector<Goal>{abduce_goal, call_goal},
                    .continuation = std::make_unique<AbduceCallContinuation>(),
                });
            }
        }

        return derivations;
    }
}
