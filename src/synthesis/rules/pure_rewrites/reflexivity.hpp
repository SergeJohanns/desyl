#pragma once

#include <rules.hpp>

namespace desyl
{
    class ReflexiveRule : public Rule
    {
    public:
        bool is_invertible() const { return true; };
        std::string name() const { return "Reflexivity"; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
