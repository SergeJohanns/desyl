#pragma once

#include <rules.hpp>

namespace desyl
{
    class OpenContinuation : public Continuation
    {
        std::vector<Clause> clauses;

    public:
        OpenContinuation(std::vector<Clause> const &clauses) : clauses(clauses){};
        Program join(std::vector<Program> const &results) const;
    };

    class OpenRule : public Rule
    {
    public:
        bool is_invertible() { return true; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
