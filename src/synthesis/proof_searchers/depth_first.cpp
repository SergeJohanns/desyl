#include <depth_first.hpp>
#include <proof_tree.hpp>

namespace desyl
{
    void depth_first_search(ProofTreeNode const &root, ProofTreeNode &current)
    {
        for (auto &child : current.children)
        {
            if (root.completed || current.completed)
            {
                return;
            }
            if (!child->expanded)
            {
                std::cout << "Using " << child->rule->name() << std::endl;
                // Rules can create expanded children, so we need to check this here
                child->expand();
            }
            depth_first_search(root, *child);
        }
        if (!current.completed)
        {
            std::cout << "Backtracking" << std::endl;
        }
    }

    std::optional<Program> DepthFirstProofSearcher::search(Goal const &goal) const
    {
        auto root = ProofTreeNode(nullptr, goal, true);
        root.make_children();
        depth_first_search(root, root);
        return root.program;
    }
}
