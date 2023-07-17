#pragma once

#include <rules.hpp>

namespace desyl
{
    class SubstRightRule : public Rule
    {
    public:
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
