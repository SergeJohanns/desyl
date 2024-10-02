#include <depth_first.hpp>
#include <proof_tree.hpp>
#include <failures/failure.hpp>

namespace desyl
{
    void depth_first_search(ProofTreeNode const &root, ProofTreeNode &current, SynthesisMode mode)
    {
        for (auto &child : current.children)
        {
            if (root.completed || current.completed)
            {
                return;
            }
            bool applied = false;
            if (!child->expanded)
            {
                if (mode != SynthesisMode::Quiet)
                {
                    std::cout << "Using " << child->rule->name() << std::endl;
                }
                // Rules can create expanded children, so we need to check this here
                try
                {
                    applied = child->expand();
                }
                catch (Failure const &)
                {
                    // Positive application of early failure rule, force backtracking
                    break;
                }
            }
            depth_first_search(root, *child, mode);
            if (applied && child->rule->is_invertible())
            {
                break;
            }
        }
        if (!current.completed && mode != SynthesisMode::Quiet)
        {
            std::cout << "Backtracking" << std::endl;
        }
    }

    std::optional<Program> DepthFirstProofSearcher::search(Goal const &goal, SynthesisMode mode) const
    {
        auto root = ProofTreeNode(nullptr, goal, true);
        root.make_children();
        depth_first_search(root, root, mode);
        return root.program;
    }
}
