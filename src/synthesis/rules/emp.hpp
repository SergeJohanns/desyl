#pragma once

#include <rules.hpp>

namespace desyl
{
    class ConstantContinuation : public Continuation
    {
        Program program;

    public:
        ConstantContinuation(Program program) : program(program) {}
        Program join(std::vector<Program> const &) const;
    };

    class EmpRule : public Rule
    {
    public:
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
