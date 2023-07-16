#pragma once

#include <rules.hpp>

namespace desyl
{
    class PreInconsistentRule : public Rule
    {
    public:
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
