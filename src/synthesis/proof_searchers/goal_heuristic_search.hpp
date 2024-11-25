#pragma once

#include <proof_searcher.hpp>
#include <priority_queue.hpp>
#include <functional>

namespace desyl
{
    std::string get_previous_rule(ProofTreeNode const &node);

    class HeuristicProofSearcher : public ProofSearcher
    {
        std::function<double(ProofTreeNode const &)> heuristic;
        void add_children_to_queue(ProofTreeNode *node, PriorityQueue &queue) const;

    public:
        HeuristicProofSearcher(std::function<double(ProofTreeNode const &)> heuristic) : heuristic(heuristic) {}
        std::optional<Program> search(ProofTreeNode &root, SynthesisMode mode) const override;
    };
}
