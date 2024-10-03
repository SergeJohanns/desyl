#pragma once

#include <proof_searcher.hpp>

namespace desyl
{
    class GuidedProofSearcher : public ProofSearcher
    {
    public:
        std::optional<Program> search(ProofTreeNode &root, SynthesisMode) const override;
    };
}
