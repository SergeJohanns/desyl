#pragma once

#include <rules.hpp>

namespace desyl
{
    class FrameRule : public Rule
    {
    public:
        bool is_invertible() const { return true; };
        std::string name() const { return "Frame"; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
