#pragma once

#include <desyl/ast.hpp>
#include <contex.hpp>

namespace desyl
{
    constexpr size_t RULES = 1;

    // TODO: Switch to AST representation
    using Program = std::string;
    class Goal
    {
    public:
        Goal(FunctionSpecification spec);
        FunctionSpecification spec;
        Vars environment;

        Vars ghosts() const;
        Vars existentials() const;
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

    class ConstantContinuation : public Continuation
    {
        Program program;

    public:
        ConstantContinuation(Program program) : program(program) {}
        Program join(std::vector<Program> const &) const;
    };

    class EmpRule : public Rule
    {
    public:
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
