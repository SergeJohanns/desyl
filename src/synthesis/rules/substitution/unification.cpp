#include <unification.hpp>

#include <algorithm>

namespace desyl
{
    std::optional<Substitutions> unify(Expression const &domain, Expression const &codomain, Vars const &variables)
    {
        if (domain == codomain)
        {
            Substitutions out;
            return out;
        }
        else if (std::holds_alternative<Identifier>(domain))
        {
            if (std::find(variables.begin(), variables.end(), std::get<Identifier>(domain)) != variables.end())
            {
                return std::optional<Substitutions>{{{std::get<Identifier>(domain), codomain}}};
            }
        }
        else if (std::holds_alternative<SetLiteral>(domain) && std::holds_alternative<SetLiteral>(codomain))
        {
            auto const &domain_set = std::get<SetLiteral>(domain);
            auto const &codomain_set = std::get<SetLiteral>(codomain);
            if (domain_set.elements.size() != codomain_set.elements.size())
            {
                return {};
            }

            Substitutions unified;
            for (size_t i = 0; i < domain_set.elements.size(); i++)
            {
                auto const &unified_element = unify(domain_set.elements[i], codomain_set.elements[i], variables);
                if (!unified_element || subtitutions_conflict(unified, *unified_element))
                {
                    return {};
                }
                unified.insert(unified_element->begin(), unified_element->end());
            }
            return unified;
        }
        else if (std::holds_alternative<UnaryOperatorCall>(domain) && std::holds_alternative<UnaryOperatorCall>(codomain))
        {
            auto const &domain_call = std::get<UnaryOperatorCall>(domain);
            auto const &codomain_call = std::get<UnaryOperatorCall>(codomain);

            if (domain_call.type == codomain_call.type)
            {
                auto const &domain_operand = domain_call.operand;
                auto const &codomain_operand = codomain_call.operand;
                return unify(*domain_operand, *codomain_operand, variables);
            }
        }
        else if (std::holds_alternative<BinaryOperatorCall>(domain) && std::holds_alternative<BinaryOperatorCall>(codomain))
        {
            auto const &domain_call = std::get<BinaryOperatorCall>(domain);
            auto const &codomain_call = std::get<BinaryOperatorCall>(codomain);

            if (domain_call.type == codomain_call.type)
            {
                auto const &domain_lhs = domain_call.lhs;
                auto const &domain_rhs = domain_call.rhs;
                auto const &codomain_lhs = codomain_call.lhs;
                auto const &codomain_rhs = codomain_call.rhs;
                auto const &unified_lhs = unify(*domain_lhs, *codomain_lhs, variables);
                if (!unified_lhs)
                {
                    return {};
                }
                auto const &unified_rhs = unify(*domain_rhs, *codomain_rhs, variables);
                if (!unified_rhs || subtitutions_conflict(*unified_lhs, *unified_rhs))
                {
                    return {};
                }
                Substitutions unified;
                unified.insert(unified_lhs->begin(), unified_lhs->end());
                unified.insert(unified_rhs->begin(), unified_rhs->end());
                return unified;
            }
        }
        return {};
    }

    std::optional<Substitutions> unify(PointerDeclaration const &domain, PointerDeclaration const &codomain, Vars const &variables)
    {
        auto const &unified_base = unify(*domain.base, *codomain.base, variables);
        if (!unified_base)
        {
            return {};
        }
        auto const &unified_value = unify(*domain.value, *codomain.value, variables);
        if (!unified_value || subtitutions_conflict(*unified_base, *unified_value))
        {
            return {};
        }
        Substitutions unified;
        unified.insert(unified_base->begin(), unified_base->end());
        unified.insert(unified_value->begin(), unified_value->end());
        return unified;
    }

    std::optional<Substitutions> unify(PredicateCall const &domain, PredicateCall const &codomain, Vars const &variables)
    {
        if (domain.name != codomain.name)
        {
            return {};
        }
        if (domain.args.size() != codomain.args.size())
        {
            return {};
        }
        Substitutions unified;
        for (size_t i = 0; i < domain.args.size(); i++)
        {
            auto const &unified_arg = unify(domain.args[i], codomain.args[i], variables);
            if (!unified_arg)
            {
                return {};
            }
            unified.insert(unified_arg->begin(), unified_arg->end());
        }
        return unified;
    }

    /// @brief Merge two unifications
    /// @param other The other unification
    /// @return The merged unification, or std::nullopt if the unifications are incompatible
    std::optional<TermUnification> TermUnification::merge(TermUnification const &other) const
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
        std::vector<size_t> new_term_index_mapping(term_index_mapping);
        new_term_index_mapping.insert(new_term_index_mapping.end(), other.term_index_mapping.begin(), other.term_index_mapping.end());
        Substitutions new_substitutions(substitutions);
        new_substitutions.insert(other.substitutions.begin(), other.substitutions.end());
        return TermUnification{
            .term_indices = new_term_indices,
            .term_index_mapping = new_term_index_mapping,
            .substitutions = new_substitutions,
        };
    }

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
                        .term_index_mapping = {i},
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

    /// @brief Unify two subheaps
    /// @param domain The domain subheap where variables can be substituted
    /// @param codomain The codomain subheap
    /// @param variables The variables to substitute
    /// @return All unifications of the subheaps
    std::vector<SubHeapUnification> unify_subheaps(Heap const &domain, Heap const &codomain, Vars const &variables)
    {
        std::vector<TermUnification> pointer_unifications = unify_all_clauses(domain.pointer_declarations, codomain.pointer_declarations, variables, unify);
        std::vector<TermUnification> predicate_unifications = unify_all_clauses(domain.predicate_calls, codomain.predicate_calls, variables, unify);
        std::vector<SubHeapUnification> result;

        for (auto const &pointer : pointer_unifications)
        {
            for (auto const &predicate : predicate_unifications)
            {
                bool any_predicate_label_higher = false;
                for (size_t i = 0; i < domain.predicate_calls.size(); i++)
                {
                    if (codomain.predicate_calls[predicate.term_index_mapping[i]].label > domain.predicate_calls[i].label)
                    {
                        any_predicate_label_higher = true;
                        break;
                    }
                }

                if (any_predicate_label_higher && !subtitutions_conflict(pointer.substitutions, predicate.substitutions))
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
}
