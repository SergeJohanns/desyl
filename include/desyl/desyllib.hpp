#pragma once

#include <desyl/ast.hpp>

namespace desyl
{
    Query parse(std::string const &input);
    void synthesize(Query query);
    std::string example(std::string const &input);
}
