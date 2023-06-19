#pragma once

#include <desyl/ast.hpp>
#include <context.hpp>
#include <synthesis.hpp>

namespace desyl
{
    // TODO: Switch to AST representation
    using Program = std::string;

    class Continuation
    {
    public:
        virtual Program join(std::vector<Program> const &results) const = 0;
    };

    struct Derivation
    {
        std::vector<Goal> goals;
        std::unique_ptr<Continuation> continuation;
    };

    class Rule
    {
    public:
        virtual std::vector<Derivation> apply(Goal const &goal) const = 0;
    };
}
