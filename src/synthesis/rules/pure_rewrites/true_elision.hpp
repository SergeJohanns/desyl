#pragma once

#include <rules.hpp>

namespace desyl
{
    class TrueElisionRule : public Rule
    {
    public:
        bool is_invertible() const { return true; };
        std::string name() const { return "TrueElision"; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
