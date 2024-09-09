#pragma once

#include <synthesis.hpp>
#include <rules.hpp>
#include <desyl/desyllib.hpp>

namespace desyl
{
    class ProofSearcher
    {
    public:
        virtual std::optional<Program> search(Goal const &goal, SynthesisMode mode) const = 0;
    };
}
