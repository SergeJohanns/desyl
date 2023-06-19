#pragma once

#include <desyl/ast.hpp>

namespace desyl
{
    /// @brief Parse a specification from a string
    /// @param input The string to parse
    /// @return The parsed specification
    Query parse(std::string const &input);

    /// @brief Synthesize a program from a specification
    /// @param query The specification to synthesize
    void synthesize(Query query);

    std::string example(std::string const &input);
}
