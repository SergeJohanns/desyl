#pragma once

#include <rules.hpp>

namespace desyl
{
    class PickRule : public Rule
    {
    public:
        bool is_invertible() const { return false; };
        std::string name() const { return "Pick"; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
