#pragma once

#include <rules.hpp>

namespace desyl
{
    class ExpandImpliedRule : public Rule
    {
    public:
        bool is_invertible() const { return true; };
        std::string name() const { return "ExpandImplied"; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
