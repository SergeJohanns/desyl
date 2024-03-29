#pragma once

#include <rules.hpp>

namespace desyl
{
    class PostInconsistentRule : public Rule
    {
    public:
        bool is_invertible() const { return false; };
        std::string name() const { return "PostInconsistent"; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
