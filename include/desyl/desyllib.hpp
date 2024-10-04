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

    /// @brief A configuration for synthesis options
    struct SynthesisConfig
    {
        /// @brief The search algorithm to use (dfs, bfs, tree)
        std::string search_algorithm;
        /// @brief The maximum depth of the search tree (-1 for no limit)
        int depth;
        /// @brief The file to write the search tree JSON to
        std::string tree_file;
        /// @brief The mode for synthesis
        SynthesisMode mode;
    };

    /// @brief Parse a specification from a string
    /// @param input The string to parse
    /// @return The parsed specification
    Query parse(std::string const &input);

    /// @brief Synthesize a program from a specification
    /// @param query The specification to synthesize
    /// @param config The configuration for synthesis
    void synthesize(Query const &query, SynthesisConfig const &config);
}
