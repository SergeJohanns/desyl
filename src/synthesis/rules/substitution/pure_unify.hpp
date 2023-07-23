#pragma once

#include <rules.hpp>

namespace desyl
{
    class PureUnifyRule : public Rule
    {
    public:
        bool is_invertible() const { return false; };
        std::string name() const { return "UnifyPure"; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
