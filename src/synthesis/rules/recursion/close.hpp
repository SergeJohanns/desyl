#pragma once

#include <rules.hpp>

namespace desyl
{
    class CloseContinuation : public Continuation
    {
        std::vector<Clause> clauses;

    public:
        CloseContinuation(std::vector<Clause> const &clauses) : clauses(clauses){};
        Program join(std::vector<Program> const &results) const;
    };

    class CloseRule : public Rule
    {
    public:
        bool is_invertible() const { return true; };
        std::string name() const { return "Close"; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
