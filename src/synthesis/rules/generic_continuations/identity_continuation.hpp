#pragma once

#include <rules.hpp>

namespace desyl
{
    class IdentityContinuation : public Continuation
    {
    public:
        Program join(std::vector<Program> const &results) const;
    };
}
