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

            bool rule_applied = false;
            // Rules can create expanded children, so we need to check this here
            if (!child->expanded)
            {
                try
                {
                    rule_applied = child->expand();
                    if (rule_applied && mode != SynthesisMode::Quiet)
                    {
                        std::cout << "Using " << child->rule->name() << std::endl;
                    }
                }
                catch (Failure const &)
                {
                    // Positive application of early failure rule, force backtracking
                    if (mode != SynthesisMode::Quiet)
                    {
                        std::cout << "Using " << child->rule->name() << std::endl;
                    }
                    break;
                }
            }
            depth_first_search(root, *child, mode);
            if (rule_applied && child->rule->is_invertible())
            {
                break;
            }
        }

        bool backtracked_rule = !current.completed && current.rule && !current.children.empty();
        if (backtracked_rule && mode != SynthesisMode::Quiet)
        {
            std::cout << "Backtracking from " << current.rule->name() << std::endl;
        }
    }

    std::optional<Program> DepthFirstProofSearcher::search(ProofTreeNode &root, SynthesisMode mode) const
    {
        root.make_children();
        depth_first_search(root, root, mode);
        return root.program;
    }
}
