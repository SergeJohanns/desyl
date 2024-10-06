#pragma once

#include <rules.hpp>

namespace desyl
{
    class AbduceCallContinuation : public Continuation
    {
    public:
        Program join(std::vector<Program> const &results) const;
    };

    class AbduceCallRule : public Rule
    {
    public:
        bool is_invertible() const { return false; };
        std::string name() const { return "AbduceCall"; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
