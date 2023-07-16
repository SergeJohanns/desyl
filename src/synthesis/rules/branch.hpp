#pragma once

#include <rules.hpp>

namespace desyl
{
    class BranchContinuation : public Continuation
    {
        Expression condition;

    public:
        BranchContinuation(Expression const &condition) : condition(condition){};
        Program join(std::vector<Program> const &results) const;
    };

    class BranchRule : public Rule
    {
        std::vector<Expression> const &learned_clauses;

    public:
        BranchRule(std::vector<Expression> const &learned_clauses) : learned_clauses(learned_clauses){};
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
