#pragma once

#include <proof_searcher.hpp>
#include <functional>
#include <queue>

namespace desyl
{
    struct PriorityProofTreeNode
    {
        ProofTreeNode *node;
        int cost;
    };

    struct PriorityProofTreeNodeComparator
    {
        bool operator()(PriorityProofTreeNode const &lhs, PriorityProofTreeNode const &rhs) const;
    };

    using PriorityQueue = std::priority_queue<PriorityProofTreeNode, std::vector<PriorityProofTreeNode>, PriorityProofTreeNodeComparator>;

    class HeuristicProofSearcher : public ProofSearcher
    {
        std::function<int(ProofTreeNode const &)> heuristic;
        void add_children_to_queue(ProofTreeNode *node, PriorityQueue &queue) const;

    public:
        HeuristicProofSearcher(std::function<int(ProofTreeNode const &)> heuristic) : heuristic(heuristic) {}
        std::optional<Program> search(ProofTreeNode &root, SynthesisMode mode) const override;
    };
}
