#pragma once

#include <rules.hpp>

namespace desyl
{
    class PostInvalidRule : public Rule
    {
        std::vector<Expression> &learned_clauses;

    public:
        bool is_invertible() const { return false; };
        PostInvalidRule(std::vector<Expression> &learned_clauses) : learned_clauses(learned_clauses){};
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
