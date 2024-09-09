#pragma once

#include <proof_searcher.hpp>

namespace desyl
{
    class BreadthFirstProofSearcher : public ProofSearcher
    {
    public:
        std::optional<Program> search(Goal const &goal) const override;
    };
}
