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
    /// @param search_algorithm The name of the search algorithm to use (dfs, bfs, tree)
    /// @param depth The maximum depth of the search tree (-1 for no limit)
    /// @param tree_file The file to write the search tree to (empty string for no output)
    /// @param mode Configuration for synthesis (Quiet, Verbose, Guided)
    void synthesize(Query query, std::string search_algorithm, int depth, std::string tree_file, SynthesisMode mode);
}
