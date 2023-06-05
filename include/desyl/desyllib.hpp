#pragma once

#include <desyl/ast.hpp>

namespace desyl
{
    Query parse(std::string const &input);
    void synthesize(Query const &query);
    std::string example(std::string const &input);
}
