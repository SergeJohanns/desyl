#pragma once

#include <optional>
#include <desyl/ast.hpp>
#include <rules.hpp>

namespace desyl
{
    std::optional<Program> with_rules(std::unique_ptr<Rule> const (&rules)[RULES], Context const &goal);
    void synthesize_query(Context const &query);
}
