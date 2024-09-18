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
    std::optional<desyl::Program> match_algorithm_name(Goal const &spec, std::string search_algorithm, SynthesisMode mode)
    {
        if (search_algorithm == "dfs")
        {
            auto search = DepthFirstProofSearcher();
            return search.search(spec, mode);
        }
        else if (search_algorithm == "bfs")
        {
            auto search = BreadthFirstProofSearcher();
            return search.search(spec, mode);
        }
        else if (search_algorithm == "tree")
        {
            auto search = WholeTreeProofSearcher();
            return search.search(spec, mode);
        }
        else if (search_algorithm.rfind("limit-", 0) == 0)
        {
            // Search algorithm is of the form "limit-X" where X is the maximum depth
            int max_depth = std::stoi(search_algorithm.substr(6));
            auto search = WholeTreeProofSearcher(max_depth);
            return search.search(spec, mode);
        }
        else
        {
            std::cerr << "Unknown search algorithm: " << search_algorithm << std::endl;
            std::cerr << "Allowed search algorithms are: " << std::endl;
            std::cerr << "  - dfs: Depth-first search" << std::endl;
            std::cerr << "  - bfs: Breadth-first search" << std::endl;
            std::cerr << "  - tree: Whole tree search" << std::endl;
            std::cerr << "  - limit-X: Whole tree up to X layers search (x is a positive integer)" << std::endl;
            return std::nullopt;
        }
    }

    /// @brief Synthesize a function from a specification
    /// @param spec The specification to synthesize
    /// @param verbose Whether to print debug information
    void synthesize_query(Goal const &spec, std::string search_algorithm, SynthesisMode mode)
    {
        std::optional<Program> result;
        if (mode == SynthesisMode::Guided)
        {
            auto search = GuidedProofSearcher();
            result = search.search(spec, mode);
        }
        else
        {
            result = match_algorithm_name(spec, search_algorithm, mode);
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
