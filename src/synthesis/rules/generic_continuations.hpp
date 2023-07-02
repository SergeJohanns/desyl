#pragma once

#include <rules.hpp>

namespace desyl
{
    class IdentityContinuation : public Continuation
    {
    public:
        Program join(std::vector<Program> const &results) const;
    };

    class ConstantContinuation : public Continuation
    {
        Program program;

    public:
        ConstantContinuation(Program program) : program(program) {}
        Program join(std::vector<Program> const &) const;
    };
}
