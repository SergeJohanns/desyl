#pragma once

#include <proof_tree.hpp>
#include <rules.hpp>
#include <desyl/desyllib.hpp>

namespace desyl
{
    class ProofSearcher
    {
    public:
        virtual std::optional<Program> search(ProofTreeNode &root, SynthesisMode mode) const = 0;
    };
}
