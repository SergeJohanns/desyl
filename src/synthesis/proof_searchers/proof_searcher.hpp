#pragma once

#include <synthesis.hpp>
#include <rules.hpp>

namespace desyl
{
    class ProofSearcher
    {
    public:
        virtual std::optional<Program> search(Goal const &goal) const = 0;
    };
}
