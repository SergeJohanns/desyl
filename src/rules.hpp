#pragma once

#include <desyl/ast.hpp>

namespace desyl
{
    constexpr size_t RULES = 1;

    // TODO: Switch to AST representation
    using Program = std::string;
    using Context = FunctionSpecification;

    class Continuation
    {
    public:
        virtual Program join(std::vector<Program> const &results) const = 0;
    };

    struct Derivation
    {
        std::vector<Context> goals;
        std::unique_ptr<Continuation> continuation;
    };

    class Rule
    {
    public:
        virtual std::vector<Derivation> apply(Context const &context) const = 0;
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
        std::vector<Derivation> apply(Context const &context) const;
    };
}
