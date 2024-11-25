#include <synthesis.hpp>

#include <iostream>
#include <fstream>
#include <optional>
#include <desyl/ast.hpp>
#include <rules.hpp>
#include <all_rules.hpp> // TODO: Remove?
#include <failures/failure.hpp>
#include <proof_searchers/depth_first.hpp>
#include <proof_searchers/breadth_first.hpp>
#include <proof_searchers/whole_tree.hpp>
#include <proof_searchers/guided.hpp>
#include <proof_searchers/goal_heuristic_search.hpp>
#include <proof_searchers/suslik_cost.hpp>
#include <proof_searchers/rule_heuristic.hpp>
#include <proof_searchers/decision_forest_heuristic.hpp>

namespace desyl
{
    /// @brief Match the search algorithm name to the correct search algorithm
    /// @param root The root of the proof tree
    /// @param search_algorithm The name of the search algorithm to use (dfs, bfs, tree)
    /// @param depth The maximum depth of the search tree (-1 for no limit)
    /// @param mode Configuration for synthesis (Quiet, Verbose, Guided)
    /// @return The synthesized program
    std::optional<desyl::Program> match_algorithm_name(ProofTreeNode &root, std::string search_algorithm, int depth, SynthesisMode mode)
    {
        if (search_algorithm == "dfs")
        {
            auto search = DepthFirstProofSearcher();
            return search.search(root, mode);
        }
        else if (search_algorithm == "bfs")
        {
            auto search = BreadthFirstProofSearcher();
            return search.search(root, mode);
        }
        else if (search_algorithm == "tree")
        {
            auto search = WholeTreeProofSearcher(depth);
            return search.search(root, mode);
        }
        else if (search_algorithm == "best")
        {
            auto search = HeuristicProofSearcher(suslik_cost);
            return search.search(root, mode);
        }
        else if (search_algorithm == "rules")
        {
            auto search = HeuristicProofSearcher(rule_heuristic);
            return search.search(root, mode);
        }
        else if (search_algorithm == "forest")
        {
            auto search = HeuristicProofSearcher(decision_forest_score);
            return search.search(root, mode);
        }
        else
        {
            throw std::runtime_error(search_algorithm + " is not a valid search algorithm");
        }
    }

    /// @brief Output the synthesized program
    /// @param signature The signature of the function
    /// @param program The synthesized program
    /// @param os The output stream to write to
    void output_program(FunctionSignature const &signature, Program const &program, std::ostream &os)
    {
        os << "void " << signature.name << "(";
        for (auto const &param : signature.args)
        {
            if (param.type == Type::Int)
            {
                os << "int ";
            }
            else if (param.type == Type::Loc)
            {
                os << "loc ";
            }
            os << param.name;
            if (&param != &signature.args.back())
            {
                os << ", ";
            }
        }
        os << ") ";
        os << program << std::endl;
    }

    /// @brief Output the proof tree to a JSON file
    /// @param node The current node in the proof tree
    /// @param os The output stream to write to
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
        if (node.expanded)
        {
            os << "\"expanded\": " << "true" << ",";
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

    /// @brief Synthesize a function from a specification
    /// @param spec The specification to synthesize
    /// @param config The configuration for synthesis
    void synthesize_query(Goal const &spec, SynthesisConfig const &config)
    {
        std::optional<Program> result;
        ProofTreeNode root = ProofTreeNode(nullptr, spec, true);
        if (config.mode == SynthesisMode::Guided)
        {
            auto search = GuidedProofSearcher();
            result = search.search(root, config.mode);
        }
        else
        {
            result = match_algorithm_name(root, config.search_algorithm, config.depth, config.mode);
        }

        if (config.mode != SynthesisMode::Quiet)
        {
            std::cout << std::endl;
        }

        if (!result.has_value())
        {
            std::cout << "No value" << std::endl;
        }
        else
        {
            output_program(spec.spec.signature, result.value(), std::cout);
        }

        if (!config.tree_file.empty())
        {
            std::ofstream file(config.tree_file);
            output_tree(root, file);
        }
    }
}
