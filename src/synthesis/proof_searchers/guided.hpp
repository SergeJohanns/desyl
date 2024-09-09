#pragma once

#include <proof_searcher.hpp>

namespace desyl
{
    class GuidedProofSearcher : public ProofSearcher
    {
    public:
        std::optional<Program> search(Goal const &goal, SynthesisMode) const override;
    };
}
