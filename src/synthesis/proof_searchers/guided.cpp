#include <guided.hpp>
#include <proof_tree.hpp>
#include <desyl/ast.hpp>

namespace desyl
{
    ProofTreeNode *get_rule_node(ProofTreeNode &current)
    {
        while (true)
        {
            std::string chosen_rule;
            std::cout << "Goal: " << stringify_function_spec(current.goal.value().spec) << std::endl;
            std::cout << "Choose a rule: ";
            std::cin >> chosen_rule;
            if (chosen_rule == "Back")
            {
                return nullptr;
            }
            for (auto &child : current.children)
            {
                if (child->rule->name() == chosen_rule)
                {
                    return child;
                }
            }
            std::cout << "Rule not found" << std::endl;
        }
    }

    ProofTreeNode *get_subderivation_node(ProofTreeNode &current)
    {
        while (true)
        {
            size_t chosen_subderivation;
            if (current.goal.has_value())
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
            if (chosen_subderivation == 0)
            {
                return nullptr;
            }
            if (chosen_subderivation > 0 && chosen_subderivation <= current.children.size())
            {
                return current.children[chosen_subderivation - 1];
            }
            std::cout << "Subderivation not found" << std::endl;
        }
    }

    void depth_first_search(ProofTreeNode const &root, ProofTreeNode &current)
    {
        if (current.children.empty())
        {
            std::cout << "No synthesis targets, returning" << std::endl;
            return;
        }
        bool backtracked = false;
        while (true)
        {
            ProofTreeNode *chosen_node = nullptr;
            if (current.children[0]->rule)
            {
                // Choose what rule to apply
                chosen_node = get_rule_node(current);
                if (chosen_node == nullptr)
                {
                    std::cout << "Backtracking" << std::endl;
                    return;
                }
            }
            else if (current.is_or_node && current.children.size() > 1)
            {
                // Choose what subderivation to follow
                chosen_node = get_subderivation_node(current);
                if (chosen_node == nullptr)
                {
                    std::cout << "Backtracking" << std::endl;
                    return;
                }
            }
            else if (backtracked)
            {
                // If the user hasn't chosen a rule or subderivation, and we already backtracked to this node, then we need to backtrack further
                return;
            }
            else if (current.is_or_node)
            {
                // This means we are in a rule node that only has one child (sometimes this is an AND node)
                chosen_node = current.children[0];
            }

            if (chosen_node == nullptr)
            {
                for (auto &child : current.children)
                {
                    // Rules can create expanded children, so we need to check this here
                    if (!child->expanded)
                    {
                        child->expand();
                    }
                    depth_first_search(root, *child);
                }
            }
            else
            {
                // Rules can create expanded children, so we need to check this here
                if (!chosen_node->expanded)
                {
                    chosen_node->expand();
                }
                depth_first_search(root, *chosen_node);
            }
            if (current.completed)
            {
                return;
            }
            // Once we have descended into a child and come back, we must have backtracked, so we need to check if we need to backtrack further
            // It's also possible that a rule was chosen that didn't create any children, but if so this is a rule decision node, so we won't backtrack anyways
            backtracked = true;
        }
    }

    std::optional<Program> GuidedProofSearcher::search(ProofTreeNode &root, SynthesisMode) const
    {
        root.make_children();
        depth_first_search(root, root);
        return root.program;
    }
}
