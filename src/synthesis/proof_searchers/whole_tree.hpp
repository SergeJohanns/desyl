#pragma once

#include <proof_searcher.hpp>

namespace desyl
{
    class WholeTreeProofSearcher : public ProofSearcher
    {
    public:
        std::optional<Program> search(Goal const &goal, SynthesisMode mode) const override;
    };
}