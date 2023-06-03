#pragma once

#include <desyl/ast.hpp>

namespace desyl
{
    Query parse(std::string const &input);
    std::string example(std::string const &input);
}
