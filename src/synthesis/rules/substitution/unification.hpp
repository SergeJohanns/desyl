#pragma once

#include <substitution.hpp>

namespace desyl
{
    std::optional<Substitutions> unify(Expression const &domain, Expression const &codomain, Vars const &variables);

    std::optional<Substitutions> unify(PointerDeclaration const &domain, PointerDeclaration const &codomain, Vars const &variables);

    std::optional<Substitutions> unify(PredicateCall const &domain, PredicateCall const &codomain, Vars const &variables);
}
