#pragma once

#include <substitution.hpp>

namespace desyl
{
    std::optional<Substitutions> unify(Expression const &domain, Expression const &codomain, Vars const &variables);

    std::optional<Substitutions> unify(PointerDeclaration const &domain, PointerDeclaration const &codomain, Vars const &variables);

    std::optional<Substitutions> unify(PredicateCall const &domain, PredicateCall const &codomain, Vars const &variables);

    /// @brief Unification of basic heap terms of the same type
    struct TermUnification
    {
        std::set<size_t> term_indices;
        std::vector<size_t> term_index_mapping;
        Substitutions substitutions;

        std::optional<TermUnification> merge(TermUnification const &other) const;
    };

    template <typename H>
    std::vector<std::vector<TermUnification>> unify_clauses(std::vector<H> const &domain, std::vector<H> const &codomain, Vars const &variables, std::optional<Substitutions> unify(H const &, H const &, Vars const &));

    template <typename H>
    std::vector<TermUnification> unify_all_clauses(std::vector<H> const &domain, std::vector<H> const &codomain, Vars const &variables, std::optional<Substitutions> unify(H const &, H const &, Vars const &));

    /// @brief Unification of two subheaps
    struct SubHeapUnification
    {
        std::set<size_t> pointer_indices;
        std::set<size_t> array_indices;
        std::set<size_t> predicate_indices;
        Substitutions substitutions;
    };

    bool predicate_label_higher(std::vector<PredicateCall> const &domain, std::vector<PredicateCall> const &codomain);

    std::vector<SubHeapUnification> unify_subheaps(Heap const &domain, Heap const &codomain, Vars const &variables);
}
