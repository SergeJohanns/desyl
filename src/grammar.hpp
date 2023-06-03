#pragma once

#include <desyl/ast.hpp>

namespace desyl
{
    std::variant<Literal, Identifier> example1(std::string const &input);
    Expression example2(std::string const &input);
}
