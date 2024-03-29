#pragma once

#include <rules.hpp>

namespace desyl
{
    class EmpRule : public Rule
    {
    public:
        bool is_invertible() const { return true; };
        std::string name() const { return "Emp"; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
