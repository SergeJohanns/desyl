#include <call.hpp>

#include <substitution/unification.hpp>
#include <algorithm>

namespace desyl
{
    Program CallContinuation::join(std::vector<Program> const &results) const
    {
        std::string call = function_name + "(";
        for (size_t i = 0; i < args.size(); i++)
        {
            call += stringify_expression(args[i]);
            if (i < args.size() - 1)
            {
                call += ", ";
            }
        }
        call += ");";
        return results[0].add_lines(call);
    }

    /// @brief Unification of basic heap terms of the same type
    struct TermUnification
    {
        std::set<size_t> term_indices;
        Substitutions substitutions;

        /// @brief Merge two unifications
        /// @param other The other unification
        /// @return The merged unification, or std::nullopt if the unifications are incompatible
        std::optional<TermUnification> merge(TermUnification const &other) const
        {
            std::set<size_t> intersection;
            std::set_intersection(
                term_indices.begin(), term_indices.end(),
                other.term_indices.begin(), other.term_indices.end(),
                std::inserter(intersection, intersection.begin()));
            if (!intersection.empty() || subtitutions_conflict(substitutions, other.substitutions))
            {
                return std::nullopt;
            }
            std::set<size_t> new_term_indices(term_indices);
            new_term_indices.insert(other.term_indices.begin(), other.term_indices.end());
            Substitutions new_substitutions(substitutions);
            new_substitutions.insert(other.substitutions.begin(), other.substitutions.end());
            return TermUnification{
                .term_indices = new_term_indices,
                .substitutions = new_substitutions,
            };
        }
    };

    /// @brief Unify all combinations of two clauses from two sets of clauses
    /// @tparam H The type of the clauses
    /// @param domain The domain clauses where variables can be substituted
    /// @param codomain The codomain clauses
    /// @param variables The variables to substitute
    /// @param unify The unification function to use
    /// @return The unifications of all clauses per domain clause
    template <typename H>
    std::vector<std::vector<TermUnification>> unify_clauses(std::vector<H> const &domain, std::vector<H> const &codomain, Vars const &variables, std::optional<Substitutions> unify(H const &, H const &, Vars const &))
    {
        std::vector<std::vector<TermUnification>> result;

        for (auto const &clause : domain)
        {
            std::vector<TermUnification> term_unifications;
            for (size_t i = 0; i < codomain.size(); i++)
            {
                auto const &unification = unify(clause, codomain[i], variables);
                if (unification.has_value())
                {
                    term_unifications.push_back(TermUnification{
                        .term_indices = {i},
                        .substitutions = unification.value(),
                    });
                }
            }
            result.push_back(std::move(term_unifications));
        }

        return result;
    }

    /// @brief Unify all combinations of clauses from two sets of clauses
    /// @tparam H The type of the clauses
    /// @param domain The domain clauses where variables can be substituted
    /// @param codomain The codomain clauses
    /// @param variables The variables to substitute
    /// @param unify The unification function to use
    /// @return The unifications of all clauses
    template <typename H>
    std::vector<TermUnification> unify_all_clauses(std::vector<H> const &domain, std::vector<H> const &codomain, Vars const &variables, std::optional<Substitutions> unify(H const &, H const &, Vars const &))
    {
        std::vector<std::vector<TermUnification>> const &unified_clauses = unify_clauses(domain, codomain, variables, unify);
        std::vector<TermUnification> result = {TermUnification{}};

        for (auto const &clause : unified_clauses)
        {
            std::vector<TermUnification> expanded;
            for (auto const &permutation : result)
            {
                for (auto const &next : clause)
                {
                    auto const &extended = permutation.merge(next);
                    if (extended.has_value())
                    {
                        expanded.push_back(extended.value());
                    }
                }
            }
            result = std::move(expanded);
        }

        return result;
    }

    /// @brief Unification of two subheaps
    struct SubHeapUnification
    {
        std::set<size_t> pointer_indices;
        std::set<size_t> array_indices;
        std::set<size_t> predicate_indices;
        Substitutions substitutions;
    };

    /// @brief Unify two predicate calls, but only if the domain is labelled with a higher label than the codomain
    /// @param domain The domain predicate call
    /// @param codomain The codomain predicate call
    /// @param variables The variables to substitute
    /// @return The substitutions, if any
    /// @note The label requirement is imposed to ensure well-foundedness of the recursion to avoid infinite loops
    std::optional<Substitutions> labelled_unify(PredicateCall const &domain, PredicateCall const &codomain, Vars const &variables)
    {
        if (domain.label >= codomain.label)
        {
            return {};
        }
        return unify(domain, codomain, variables);
    }

    /// @brief Unify two subheaps
    /// @param domain The domain subheap where variables can be substituted
    /// @param codomain The codomain subheap
    /// @param variables The variables to substitute
    /// @return All unifications of the subheaps
    std::vector<SubHeapUnification> unify_subheaps(Heap const &domain, Heap const &codomain, Vars const &variables)
    {
        std::vector<TermUnification> pointer_unifications = unify_all_clauses(domain.pointer_declarations, codomain.pointer_declarations, variables, unify);
        std::vector<TermUnification> predicate_unifications = unify_all_clauses(domain.predicate_calls, codomain.predicate_calls, variables, labelled_unify);
        std::vector<SubHeapUnification> result;

        for (auto const &pointer : pointer_unifications)
        {
            for (auto const &predicate : predicate_unifications)
            {
                if (!subtitutions_conflict(pointer.substitutions, predicate.substitutions))
                {
                    Substitutions unified;
                    unified.insert(pointer.substitutions.begin(), pointer.substitutions.end());
                    unified.insert(predicate.substitutions.begin(), predicate.substitutions.end());
                    result.push_back(SubHeapUnification{
                        .pointer_indices = pointer.term_indices,
                        .array_indices = {},
                        .predicate_indices = predicate.term_indices,
                        .substitutions = unified,
                    });
                }
            }
        }

        return result;
    }

    /// @brief Execute the effects of a subheap unification
    /// @param unification The unification to execute
    /// @return The derivation of the effects, if any
    std::optional<Derivation> execute_subheap_unification(SubHeapUnification const &unification, Goal const &goal, FunctionSpecification const &spec, Vars const &environment)
    {
        std::vector<Expression> args;
        Vars arg_variables;
        for (auto const &var : spec.signature.args)
        {
            args.push_back(substitute(var.name, unification.substitutions));
            vars(args.back(), arg_variables);
        }
        if (!std::includes(environment.begin(), environment.end(), arg_variables.begin(), arg_variables.end()))
        {
            return std::nullopt;
        }

        Goal new_goal(goal);
        std::vector<size_t> pointer_indices(unification.pointer_indices.begin(), unification.pointer_indices.end());
        std::sort(pointer_indices.begin(), pointer_indices.end(), std::greater<size_t>());
        for (auto const &index : pointer_indices)
        {
            new_goal.spec.precondition.heap.pointer_declarations.erase(new_goal.spec.precondition.heap.pointer_declarations.begin() + index);
        }
        std::vector<size_t> predicate_indices(unification.predicate_indices.begin(), unification.predicate_indices.end());
        std::sort(predicate_indices.begin(), predicate_indices.end(), std::greater<size_t>());
        for (auto const &index : predicate_indices)
        {
            new_goal.spec.precondition.heap.predicate_calls.erase(new_goal.spec.precondition.heap.predicate_calls.begin() + index);
        }
        auto const &consequent = substitute(spec.postcondition, unification.substitutions);
        new_goal.spec.precondition.proposition.insert(new_goal.spec.precondition.proposition.end(), consequent.proposition.begin(), consequent.proposition.end());
        new_goal.spec.precondition.heap.pointer_declarations.insert(new_goal.spec.precondition.heap.pointer_declarations.end(), consequent.heap.pointer_declarations.begin(), consequent.heap.pointer_declarations.end());
        new_goal.spec.precondition.heap.predicate_calls.insert(new_goal.spec.precondition.heap.predicate_calls.end(), consequent.heap.predicate_calls.begin(), consequent.heap.predicate_calls.end());

        return Derivation{
            .goals = std::vector<Goal>{new_goal},
            .continuation = std::make_unique<CallContinuation>(spec.signature.name, args),
        };
    }

    std::vector<Derivation> CallRule::apply(Goal const &goal) const
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
            auto unifications = unify_subheaps(spec.precondition.heap, goal.spec.precondition.heap, variables);

            for (auto unification : unifications)
            {
                auto derivation = execute_subheap_unification(unification, goal, spec, goal.environment);
                if (derivation.has_value())
                {
                    derivations.push_back(std::move(derivation.value()));
                }
            }
        }
        if (!derivations.empty())
        {
            std::cout << "Using CALL" << std::endl;
        }
        return derivations;
    }
}
