#pragma once

#include <proof_searcher.hpp>

namespace desyl
{
    class WholeTreeProofSearcher : public ProofSearcher
    {
        int max_depth;

    public:
        WholeTreeProofSearcher(int max_depth = -1) : max_depth(max_depth) {}
        std::optional<Program> search(Goal const &goal, SynthesisMode mode) const override;
    };
}
