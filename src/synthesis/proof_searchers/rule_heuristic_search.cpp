#include <rule_heuristic_search.hpp>
#include <failures/failure.hpp>

namespace desyl
{
    void RuleHeuristicProofSearcher::add_children_to_queue(ProofTreeNode *node, PriorityQueue &queue) const
    {
        for (auto &child : node->children)
        {
            if (child->rule != nullptr)
            {
                queue.push({child, heuristic(*node, child->rule->name())});
            }
            else
            {
                // If the child has no rule, it's an intermediate node, so we need to add its children to the queue
                add_children_to_queue(child, queue);
            }
        }
    }

    std::optional<Program> RuleHeuristicProofSearcher::search(ProofTreeNode &root, SynthesisMode mode) const
    {
        root.make_children();
        PriorityQueue queue = PriorityQueue(PriorityProofTreeNodeComparator());
        add_children_to_queue(&root, queue);
        while (!root.completed && !queue.empty())
        {
            auto [node, cost] = queue.top();
            queue.pop();
            if (mode != SynthesisMode::Quiet)
            {
                std::cout << "Rule: " << node->rule->name() << std::endl;
                std::cout << "Priority: " << cost << std::endl;
            }
            try
            {
                node->expand();
                add_children_to_queue(node, queue);
            }
            catch (Failure const &)
            {
                break;
            }
        }
        return root.program;
    }
}
