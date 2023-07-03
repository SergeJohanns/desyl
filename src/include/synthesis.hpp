#pragma once

#include <optional>
#include <desyl/ast.hpp>
#include <context.hpp>

namespace desyl
{
    struct VariableSnapshot
    {
        Vars precondition;
        Vars environment;
        Vars postcondition;

        Vars all() const;
        Vars ghosts() const;
        Vars existentials() const;
    };

    struct Goal
    {
        Goal(FunctionSpecification spec);
        FunctionSpecification spec;
        Vars environment;
        VariableSnapshot variables() const;
    };

    void synthesize_query(Goal const &query);
}
