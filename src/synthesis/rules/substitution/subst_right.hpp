#pragma once

#include <rules.hpp>

namespace desyl
{
    class SubstRightRule : public Rule
    {
    public:
        bool is_invertible() const { return false; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
