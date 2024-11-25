#include <priority_queue.hpp>

namespace desyl
{
    bool PriorityProofTreeNodeComparator::operator()(PriorityProofTreeNode const &lhs, PriorityProofTreeNode const &rhs) const
    {
        return lhs.cost > rhs.cost; // Lower cost is higher priority, and the priority queue expects true if rhs has higher priority
    }
}
