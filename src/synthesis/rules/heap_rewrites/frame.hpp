#pragma once

#include <rules.hpp>

namespace desyl
{
    class FrameRule : public Rule
    {
    public:
        bool is_invertible() { return true; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}