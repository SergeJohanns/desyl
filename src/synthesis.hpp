#pragma once

#include <optional>
#include <desyl/ast.hpp>

namespace desyl
{
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

    std::optional<Program> with_rules(std::vector<std::unique_ptr<Rule>> const &rules, Context const &goal);
    void synthesize_query(Context const &query);

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
