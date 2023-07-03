#pragma once

#include <rules.hpp>

namespace desyl
{
    class ReadContinuation : public Continuation
    {
        Line line;

    public:
        ReadContinuation(Identifier const &output, PointerDeclaration const &pointer);
        Program join(std::vector<Program> const &results) const;
    };

    class ReadRule : public Rule
    {
    public:
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
