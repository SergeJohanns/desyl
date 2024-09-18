#include <whole_tree.hpp>
#include <proof_tree.hpp>
#include <failures/failure.hpp>
#include <fstream>

namespace desyl
{
    void output_tree(ProofTreeNode const &node, std::ostream &os)
    {
        os << "{";
        if (node.rule)
        {
            os << "\"rule\": \"" << node.rule->name() << "\",";
        }
        else if (node.goal.has_value())
        {
            os << "\"goal\": \"" << stringify_function_spec(node.goal->spec) << "\",";
        }
        if (node.completed)
        {
            os << "\"completed\": " << "true" << ",";
        }
        os << "\"children\": [";
        for (auto &child : node.children)
        {
            output_tree(*child, os);
            if (&child != &node.children.back())
            {
                os << ",";
            }
        }
        os << "]";
        os << "}";
    }

    std::optional<Program> WholeTreeProofSearcher::search(Goal const &goal, SynthesisMode mode) const
    {
        auto root = ProofTreeNode(nullptr, goal, true);
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
        if (mode != SynthesisMode::Quiet)
        {
            std::cout << "Output whole tree to JSON? (y/N): ";
            char c;
            std::cin >> c;
            if (c == 'y')
            {
                std::ofstream file("proof_tree.json");
                output_tree(root, file);
            }
        }
        return root.program;
    }
}
