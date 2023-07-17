#pragma once

#include <rules.hpp>

namespace desyl
{
    class EmptyContinuation : public Continuation
    {
    public:
        Program join(std::vector<Program> const &) const;
    };
}
