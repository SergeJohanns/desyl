#pragma once

#include <rules.hpp>

namespace desyl
{
    class IdentityContinuation : public Continuation
    {
    public:
        Program join(std::vector<Program> const &results) const;
    };

    class FrameRule : public Rule
    {
    public:
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
