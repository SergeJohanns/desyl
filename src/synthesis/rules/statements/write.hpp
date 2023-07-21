#pragma once

#include <rules.hpp>

namespace desyl
{
    class WriteContinuation : public Continuation
    {
        Line line;

    public:
        WriteContinuation(PointerDeclaration const &pointer);
        Program join(std::vector<Program> const &results) const;
    };

    class WriteRule : public Rule
    {
    public:
        bool is_invertible() const { return false; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
