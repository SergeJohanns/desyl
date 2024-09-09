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
    /// @brief Synthesize a function from a specification
    /// @param spec The specification to synthesize
    /// @param verbose Whether to print debug information
    void synthesize_query(Goal const &spec, SynthesisMode mode)
    {
        std::optional<Program> result;
        if (mode == SynthesisMode::Guided)
        {
            auto search = GuidedProofSearcher();
            result = search.search(spec, mode);
        }
        else
        {
            auto search = WholeTreeProofSearcher();
            result = search.search(spec, mode);
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
