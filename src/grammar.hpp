#pragma once

#include <desyl/ast.hpp>

namespace desyl
{
    Expression parse_expr(std::string const &input);
    Heap parse_heap(std::string const &input);
}
