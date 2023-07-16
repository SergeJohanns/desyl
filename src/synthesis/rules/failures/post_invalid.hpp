#pragma once

#include <rules.hpp>

namespace desyl
{
    class PostInvalidRule : public Rule
    {
        std::vector<Expression> &learned_clauses;

    public:
        PostInvalidRule(std::vector<Expression> &learned_clauses) : learned_clauses(learned_clauses){};
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
