#pragma once

#include <desyl/ast.hpp>
#include <context.hpp>
#include <synthesis.hpp>

namespace desyl
{
    constexpr size_t RULES = 4;

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

    class IdentityContinuation : public Continuation
    {
    public:
        Program join(std::vector<Program> const &results) const;
    };

    class FrameRule : public Rule
    {
    public:
        std::vector<Derivation> apply(Goal const &goal) const;
    };

    class WriteContinuation : public Continuation
    {
        Program program;

    public:
        WriteContinuation(PointerDeclaration const &pointer);
        Program join(std::vector<Program> const &results) const;
    };

    class WriteRule : public Rule
    {
    public:
        std::vector<Derivation> apply(Goal const &goal) const;
    };

    class ReadContinuation : public Continuation
    {
        Program program;

    public:
        ReadContinuation(Identifier const &output, PointerDeclaration const &pointer);
        Program join(std::vector<Program> const &results) const;
    };

    class ReadRule : public Rule
    {
    public:
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
