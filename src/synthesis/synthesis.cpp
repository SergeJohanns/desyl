#include <synthesis.hpp>

#include <iostream>
#include <optional>
#include <desyl/ast.hpp>
#include <rules.hpp>
#include <all_rules.hpp> // TODO: Remove?
#include <failures/failure.hpp>
#include <proof_searchers/depth_first.hpp>

namespace desyl
{
    /// @brief Synthesize a function from a specification
    /// @param spec The specification to synthesize
    /// @param verbose Whether to print debug information
    void synthesize_query(Goal const &spec, SynthesisMode mode)
    {
        auto search = DepthFirstProofSearcher();
        auto res = search.search(spec);
        if (mode != SynthesisMode::Quiet)
        {
            std::cout << std::endl;
        }
        if (res.has_value())
        {
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
            std::cout << res.value() << std::endl;
        }
        else
        {
            std::cout << "No value" << std::endl;
        }
    }
}
