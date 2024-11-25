#pragma once

#include <queue>
#include <proof_tree.hpp>

namespace desyl
{
    struct PriorityProofTreeNode
    {
        ProofTreeNode *node;
        double cost;
    };

    struct PriorityProofTreeNodeComparator
    {
        bool operator()(PriorityProofTreeNode const &lhs, PriorityProofTreeNode const &rhs) const;
    };

    using PriorityQueue = std::priority_queue<PriorityProofTreeNode, std::vector<PriorityProofTreeNode>, PriorityProofTreeNodeComparator>;

}
