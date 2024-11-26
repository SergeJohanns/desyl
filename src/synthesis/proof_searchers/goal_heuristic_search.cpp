#include <goal_heuristic_search.hpp>
#include <failures/failure.hpp>

namespace desyl
{
    std::string get_previous_rule(ProofTreeNode const &node)
    {
        ProofTreeNode const *current = node.parent;
        while (current != nullptr)
        {
            if (current->rule != nullptr)
            {
                return current->rule->name();
            }
            current = current->parent;
        }
        return "";
    }

    void GoalHeuristicProofSearcher::add_children_to_queue(ProofTreeNode *node, PriorityQueue &queue) const
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

    std::optional<Program> GoalHeuristicProofSearcher::search(ProofTreeNode &root, SynthesisMode mode) const
    {
        root.make_children();
        PriorityQueue queue = PriorityQueue(PriorityProofTreeNodeComparator());
        queue.push({&root, 0}); // The root doens't need a priority, since it's the only one in the queue
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
