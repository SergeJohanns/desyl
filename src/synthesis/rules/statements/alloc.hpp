#pragma once

#include <rules.hpp>

namespace desyl
{
    class AllocContinuation : public Continuation
    {
        Line line;

    public:
        AllocContinuation(Identifier const &output, Literal const &size);
        Program join(std::vector<Program> const &results) const;
    };

    class AllocRule : public Rule
    {
    public:
        bool is_invertible() const { return false; };
        std::string name() const { return "Alloc"; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
