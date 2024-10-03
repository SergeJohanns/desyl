#include <synthesis.hpp>

#include <iostream>
#include <optional>
#include <desyl/ast.hpp>
#include <rules.hpp>
#include <all_rules.hpp> // TODO: Remove?
#include <failures/failure.hpp>
#include <proof_searchers/depth_first.hpp>
#include <proof_searchers/breadth_first.hpp>
#include <proof_searchers/whole_tree.hpp>
#include <proof_searchers/guided.hpp>

namespace desyl
{
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
        else
        {
            std::cerr << "Unknown search algorithm: " << search_algorithm << std::endl;
            std::cerr << "Allowed search algorithms are: " << std::endl;
            std::cerr << "  - dfs: Depth-first search" << std::endl;
            std::cerr << "  - bfs: Breadth-first search" << std::endl;
            std::cerr << "  - tree: Whole tree search" << std::endl;
            return std::nullopt;
        }
    }

    /// @brief Synthesize a function from a specification
    /// @param spec The specification to synthesize
    /// @param search_algorithm The name of the search algorithm to use (dfs, bfs, tree)
    /// @param depth The maximum depth of the search tree (-1 for no limit)
    /// @param mode Configuration for synthesis (Quiet, Verbose, Guided)
    void synthesize_query(Goal const &spec, std::string search_algorithm, int depth, SynthesisMode mode)
    {
        std::optional<Program> result;
        ProofTreeNode root = ProofTreeNode(nullptr, spec, true);
        if (mode == SynthesisMode::Guided)
        {
            auto search = GuidedProofSearcher();
            result = search.search(root, mode);
        }
        else
        {
            result = match_algorithm_name(root, search_algorithm, depth, mode);
        }

        if (mode != SynthesisMode::Quiet)
        {
            std::cout << std::endl;
        }

        if (!result.has_value())
        {
            std::cout << "No value" << std::endl;
            return;
        }

        std::cout << "void " << spec.spec.signature.name << "(";
        for (auto const &param : spec.spec.signature.args)
        {
            if (param.type == Type::Int)
            {
                std::cout << "int ";
            }
            else if (param.type == Type::Loc)
            {
                std::cout << "loc ";
            }
            std::cout << param.name;
            if (&param != &spec.spec.signature.args.back())
            {
                std::cout << ", ";
            }
        }
        std::cout << ") ";
        std::cout << result.value() << std::endl;
    }
}
