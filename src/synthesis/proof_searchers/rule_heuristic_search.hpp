#pragma once

#include <proof_searcher.hpp>
#include <priority_queue.hpp>
#include <functional>

namespace desyl
{
    class RuleHeuristicProofSearcher : public ProofSearcher
    {
        std::function<double(ProofTreeNode const &, std::string)> heuristic;
        void add_children_to_queue(ProofTreeNode *node, PriorityQueue &queue) const;

    public:
        RuleHeuristicProofSearcher(std::function<double(ProofTreeNode const &, std::string)> heuristic) : heuristic(heuristic) {}
        std::optional<Program> search(ProofTreeNode &root, SynthesisMode mode) const override;
    };
}
