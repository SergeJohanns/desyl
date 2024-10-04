#pragma once

#include <optional>
#include <synthesis.hpp>
#include <rules.hpp>

namespace desyl
{
    class ProofTreeNode
    {
        bool complete_intermediate(std::vector<Program> const &child_program);
        Program get_program(std::vector<Program> child_program) const; // Returns the program represented by the tree once it is complete

    public:
        ProofTreeNode(ProofTreeNode *parent, std::optional<Goal> goal, bool is_or_node); // Already expanded node, e.g. root or subderivation
        ProofTreeNode(ProofTreeNode *parent, std::shared_ptr<Rule> rule);                // Node with pending rule application that still needs to be expanded
        ~ProofTreeNode();
        bool is_or_node;
        bool expanded;
        bool completed;
        std::optional<Goal> goal;
        std::optional<Program> program;
        std::unique_ptr<Continuation> continuation;
        std::shared_ptr<Rule> rule;
        ProofTreeNode *parent;
        std::vector<ProofTreeNode *> children;
        bool expand();
        void make_children();
        bool complete(); // Returns true if the entire tree is complete
        // void error(); // TODO
    };
}
