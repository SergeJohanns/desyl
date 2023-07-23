#pragma once

#include <rules.hpp>

namespace desyl
{
    class PureFrameRule : public Rule
    {
    public:
        bool is_invertible() const { return true; };
        std::string name() const { return "PureFrame"; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
