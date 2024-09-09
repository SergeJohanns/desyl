#include <breadth_first.hpp>
#include <proof_tree.hpp>

namespace desyl
{
    std::optional<Program> BreadthFirstProofSearcher::search(Goal const &goal) const
    {
        auto root = ProofTreeNode(nullptr, goal, true);
        root.make_children();
        int depth = 0;
        std::vector<ProofTreeNode *> layer = {&root};
        while (!root.completed && !layer.empty())
        {
            std::cout << "Depth: " << depth++ << std::endl;
            std::cout << "Layer size: " << layer.size() << std::endl;
            std::vector<ProofTreeNode *> next_layer;
            for (auto &node : layer)
            {
                if (root.completed)
                {
                    break;
                }
                if (node->completed)
                {
                    continue;
                }
                if (!node->expanded)
                {
                    node->expand();
                }
                for (auto &child : node->children)
                {
                    next_layer.push_back(child);
                }
            }
            layer = next_layer;
        }
        return root.program;
    }
}
