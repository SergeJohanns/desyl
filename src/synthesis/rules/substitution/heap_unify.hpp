#pragma once

#include <rules.hpp>

namespace desyl
{
    class HeapUnifyRule : public Rule
    {
    public:
        bool is_invertible() const { return false; };
        std::string name() const { return "UnifyHeaps"; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
