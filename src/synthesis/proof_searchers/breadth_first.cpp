#include <breadth_first.hpp>
#include <proof_tree.hpp>
#include <failures/failure.hpp>

namespace desyl
{
    // Each nested vector contains direct siblings, used for backtracking
    using Layer = std::vector<std::vector<ProofTreeNode *>>;

    std::optional<Program> BreadthFirstProofSearcher::search(Goal const &goal, SynthesisMode mode) const
    {
        auto root = ProofTreeNode(nullptr, goal, true);
        root.make_children();
        int depth = 0;
        Layer layer = {{&root}};
        while (!root.completed && !layer.empty())
        {
            if (mode != SynthesisMode::Quiet)
            {
                std::cout << "Depth: " << depth++ << std::endl;
                std::size_t layer_width = 0;
                for (auto const &siblings : layer)
                {
                    layer_width += siblings.size();
                }
                std::cout << "Layer size: " << layer_width << std::endl;
            }
            Layer next_layer;
            for (auto const &siblings : layer)
            {
                Layer sibling_children;
                bool backtrack = false;
                for (auto &node : siblings)
                {
                    if (node->completed)
                    {
                        continue;
                    }
                    if (!node->expanded)
                    {
                        try
                        {
                            node->expand();
                            if (root.completed)
                            {
                                return root.program;
                            }
                        }
                        catch (Failure const &)
                        {
                            if (mode == SynthesisMode::Verbose)
                            {
                                std::cout << "Using " << node->rule->name() << std::endl;
                            }
                            backtrack = true;
                            break;
                        }
                    }
                    sibling_children.push_back(node->children);
                }
                if (!backtrack)
                {
                    next_layer.insert(next_layer.end(), sibling_children.begin(), sibling_children.end());
                }
            }
            layer = next_layer;
        }
        return root.program;
    }
}
