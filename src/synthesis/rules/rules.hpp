#pragma once

#include <desyl/ast.hpp>
#include <context.hpp>
#include <synthesis.hpp>
#include <iostream>

namespace desyl
{
    using Line = std::string;

    // TODO: Switch to AST representation
    class Program
    {
        std::string outer;
        std::string code;
        int indentation_level = 1;

    public:
        Program(std::string outer) : outer(outer) {}
        // Can't modify the program itself because the old version may be needed for backtracking
        Program add_lines(Line const &line) const;
        friend std::ostream &operator<<(std::ostream &os, const Program &program);
    };

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
