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
        std::string outer;

    public:
        ConstantContinuation(FunctionSignature const &signature);
        Program join(std::vector<Program> const &) const;
    };
}
