#pragma once

#include <proof_searcher.hpp>

namespace desyl
{
    class DepthFirstProofSearcher : public ProofSearcher
    {
    public:
        std::optional<Program> search(ProofTreeNode &root, SynthesisMode mode) const override;
    };
}
