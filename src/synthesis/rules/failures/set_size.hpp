#pragma once

#include <rules.hpp>

namespace desyl
{
    class SetSizeRule : public Rule
    {
    public:
        bool is_invertible() const { return false; };
        std::string name() const { return "SetSize"; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
