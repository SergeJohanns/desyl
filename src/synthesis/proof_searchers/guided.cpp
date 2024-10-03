#include <guided.hpp>
#include <proof_tree.hpp>
#include <desyl/ast.hpp>

namespace desyl
{
    void depth_first_search(ProofTreeNode const &root, ProofTreeNode &current)
    {
        if (current.children.empty())
        {
            std::cout << "Backtracking" << std::endl;
            return;
        }
        std::string chosen_rule;
        int chosen_subderivation;
        // Rule choice
        if (current.children[0]->rule)
        {
            std::cout << "Goal: " << stringify_function_spec(current.goal.value().spec) << std::endl;
            std::cout << "Choose a rule: ";
            std::cin >> chosen_rule;
        }
        else if (current.is_or_node && current.children[0]->goal.has_value())
        {
            if (current.children.size() == 1)
            {
                chosen_subderivation = 1;
            }
            else
            {
                if (!current.rule)
                {
                    std::cout << "Goal: " << stringify_function_spec(current.goal.value().spec) << std::endl;
                }
                int i = 1;
                for (auto &child : current.children)
                {
                    std::cout << "Subderivation " << i++ << ": " << stringify_function_spec(child->goal.value().spec) << std::endl;
                }
                std::cout << "Choose a subderivation: ";
                std::cin >> chosen_subderivation;
            }
        }
        int i = 1;
        for (auto &child : current.children)
        {
            bool expanded = false;
            if (root.completed || current.completed)
            {
                return;
            }
            if (!child->expanded)
            {
                if (child->rule->name() != chosen_rule)
                {
                    continue;
                }
                std::cout << "Using " << child->rule->name() << std::endl;
                // Rules can create expanded children, so we need to check this here
                child->expand();
                expanded = true;
            }
            if (expanded || chosen_subderivation == 0 || i == chosen_subderivation)
            {
                depth_first_search(root, *child);
            }
            i++;
        }
    }

    std::optional<Program> GuidedProofSearcher::search(ProofTreeNode &root, SynthesisMode) const
    {
        root.make_children();
        depth_first_search(root, root);
        return root.program;
    }
}
