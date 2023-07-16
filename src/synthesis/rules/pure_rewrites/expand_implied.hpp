#pragma once

#include <rules.hpp>

namespace desyl
{
    class ExpandImpliedRule : public Rule
    {
    public:
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
