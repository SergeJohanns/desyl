#pragma once

#include <rules.hpp>

namespace desyl
{
    class FreeContinuation : public Continuation
    {
        Line line;

    public:
        FreeContinuation(Expression const &location);
        Program join(std::vector<Program> const &results) const;
    };

    class FreeRule : public Rule
    {
    public:
        bool is_invertible() const { return false; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
