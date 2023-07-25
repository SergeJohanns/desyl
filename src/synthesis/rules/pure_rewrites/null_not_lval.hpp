#pragma once

#include <rules.hpp>

namespace desyl
{
    class NullNotLValRule : public Rule
    {
    public:
        bool is_invertible() const { return true; };
        std::string name() const { return "NullNotLVal"; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
