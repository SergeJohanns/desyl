#pragma once

#include <rules.hpp>

namespace desyl
{
    class WriteContinuation : public Continuation
    {
        Program program;

    public:
        WriteContinuation(PointerDeclaration const &pointer);
        Program join(std::vector<Program> const &results) const;
    };

    class WriteRule : public Rule
    {
    public:
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
