#pragma once

#include <desyl/ast.hpp>

namespace desyl
{
    /// @brief Specifies whether to print debug information and whether to ask for user input
    enum class SynthesisMode
    {
        Quiet,
        Verbose,
        Guided
    };

    /// @brief Parse a specification from a string
    /// @param input The string to parse
    /// @return The parsed specification
    Query parse(std::string const &input);

    /// @brief Synthesize a program from a specification
    /// @param query The specification to synthesize
    void synthesize(Query query, SynthesisMode mode);
}
