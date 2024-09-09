#include <whole_tree.hpp>
#include <proof_tree.hpp>

namespace desyl
{
    std::optional<Program> WholeTreeProofSearcher::search(Goal const &goal, SynthesisMode mode) const
    {
        auto root = ProofTreeNode(nullptr, goal, true);
        root.make_children();
        int depth = 0;
        std::vector<ProofTreeNode *> layer = {&root};
        while (!layer.empty())
        {
            if (mode != SynthesisMode::Quiet)
            {
                std::cout << "Depth: " << depth++ << std::endl;
                std::cout << "Layer size: " << layer.size() << std::endl;
            }
            std::vector<ProofTreeNode *> next_layer;
            for (auto &node : layer)
            {
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
