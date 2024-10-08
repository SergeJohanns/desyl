#include <proof_tree.hpp>

#include <all_rules.hpp>

namespace desyl
{
    std::vector<std::shared_ptr<Rule>> all_rules_vector = all_rules(); // Only call once, because MEMORY DOES NOT GET FREED

    ProofTreeNode::ProofTreeNode(ProofTreeNode *parent, std::optional<Goal> goal, bool is_or_node)
    {
        this->is_or_node = is_or_node;
        this->expanded = true;
        this->completed = false;
        this->goal = goal;
        this->parent = parent;
    }

    ProofTreeNode::ProofTreeNode(ProofTreeNode *parent, std::shared_ptr<Rule> rule)
    {
        this->is_or_node = true;
        this->expanded = false;
        this->completed = false;
        this->rule = rule;
        this->parent = parent;
    }

    ProofTreeNode::~ProofTreeNode()
    {
        for (auto &child : this->children)
        {
            delete child;
        }
    }

    /// @brief Expand the node by applying the rule
    /// @return True if the node was expanded, false if the rule did not apply
    bool ProofTreeNode::expand()
    {
        if (expanded)
        {
            throw std::runtime_error("Cannot expand already expanded node");
        }

        expanded = true;
        // This could throw a Failure if an early failure rule triggers, but that's for the strategy to handle
        auto derivations = rule->apply(parent->goal.value());
        if (derivations.empty())
        {
            return false;
        }
        this->children.reserve(derivations.size());

        for (auto &derivation : derivations)
        {
            if (derivation.goals.empty())
            {
                // Complete on successful EMP rule application
                this->continuation = std::move(derivation.continuation);
                this->complete();
            }
            else if (derivation.goals.size() == 1)
            {
                // No need to create a new AND node for a single goal
                auto child = new ProofTreeNode(this, std::move(derivation.goals[0]), true);
                child->continuation = std::move(derivation.continuation);
                child->make_children();
                this->children.push_back(child);
            }
            else
            {
                auto derivation_node = new ProofTreeNode(this, std::nullopt, false);
                derivation_node->continuation = std::move(derivation.continuation);
                for (auto const &goal : derivation.goals)
                {
                    auto child = new ProofTreeNode(derivation_node, goal, true);
                    child->make_children();
                    derivation_node->children.push_back(child);
                }
                this->children.push_back(derivation_node);
            }
        }
        return true;
    }

    /// @brief Create children for the node corresponding to the different rules that can be applied
    void ProofTreeNode::make_children()
    {
        if (!expanded)
        {
            throw std::runtime_error("Cannot make children of an unexpanded node");
        }

        for (auto &rule : all_rules_vector)
        {
            this->children.push_back(new ProofTreeNode(this, rule));
        }
    }

    /// @brief Complete the node by combining the programs of the children, and propagate the completion up the tree
    /// @return True if the entire tree is complete
    bool ProofTreeNode::complete()
    {
        // Only ever applied for an applicable EMP rule, so no need to check anything
        return this->complete_intermediate({});
    }

    /// @brief Internal function to complete the node and propagate the partial program up the tree
    /// @param child_program The program of the child that triggered completion
    /// @return True if the entire tree is complete
    bool ProofTreeNode::complete_intermediate(std::vector<Program> const &child_program)
    {
        if (!this->is_or_node)
        {
            for (auto &child : this->children)
            {
                if (!child->completed)
                {
                    return false;
                }
            }
        }
        this->completed = true;
        this->program = this->get_program(child_program);
        if (this->parent)
        {
            return this->parent->complete_intermediate({this->program.value()});
        }
        else
        {
            return true;
        }
    }

    /// @brief Get the program represented by this subtree given the programs of the children
    /// @param child_program The programs of the children
    /// @return The program represented by this subtree
    Program ProofTreeNode::get_program(std::vector<Program> child_program) const
    {
        if (!this->continuation)
        {
            return child_program[0]; // The root and the OR nodes below an AND node have no continuation and should just pass the program up
        }
        else if (this->is_or_node)
        {
            return this->continuation->join(child_program); // The child that triggers completion is the only one that matters
        }
        else
        {
            std::vector<Program> child_programs;
            for (auto &child : this->children)
            {
                child_programs.push_back(child->program.value()); // .value() is allowed because by this point all children are completed
            }
            return this->continuation->join(child_programs);
        }
    }
}
