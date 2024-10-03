#include <whole_tree.hpp>
#include <proof_tree.hpp>
#include <failures/failure.hpp>

namespace desyl
{
    std::optional<Program> WholeTreeProofSearcher::search(ProofTreeNode &root, SynthesisMode mode) const
    {
        root.make_children();
        int depth = 0;
        std::vector<ProofTreeNode *> layer = {&root};
        // If max_depth is -1, search until completion
        while (!layer.empty() && depth != this->max_depth)
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
                    try
                    {
                        node->expand();
                    }
                    catch (Failure const &)
                    {
                        // Positive application of early failure rule, keep going
                        continue;
                    }
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
