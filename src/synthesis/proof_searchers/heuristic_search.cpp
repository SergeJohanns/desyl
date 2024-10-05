#include <heuristic_search.hpp>
#include <failures/failure.hpp>

namespace desyl
{
    bool PriorityProofTreeNodeComparator::operator()(PriorityProofTreeNode const &lhs, PriorityProofTreeNode const &rhs) const
    {
        return lhs.cost > rhs.cost; // Lower cost is higher priority, and the priority queue expects true if rhs has higher priority
    }

    void HeuristicProofSearcher::add_children_to_queue(ProofTreeNode *node, PriorityQueue &queue) const
    {
        for (auto &child : node->children)
        {
            if (child->goal.has_value())
            {
                queue.push({child, heuristic(*child)});
            }
            add_children_to_queue(child, queue);
        }
    }

    std::optional<Program> HeuristicProofSearcher::search(ProofTreeNode &root, SynthesisMode mode) const
    {
        root.make_children();
        PriorityQueue queue = PriorityQueue(PriorityProofTreeNodeComparator());
        queue.push({&root, heuristic(root)});
        while (!root.completed && !queue.empty())
        {
            auto [node, cost] = queue.top();
            queue.pop();
            if (mode != SynthesisMode::Quiet)
            {
                std::cout << "Goal: " << stringify_function_spec(node->goal->spec) << std::endl;
                std::cout << "Priority: " << cost << std::endl;
            }
            for (auto &child : node->children)
            {
                if (node->completed)
                {
                    break;
                }

                try
                {
                    child->expand();
                    add_children_to_queue(child, queue);
                }
                catch (Failure const &)
                {
                    break;
                }
            }
        }
        return root.program;
    }
}
