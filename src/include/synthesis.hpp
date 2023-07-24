#pragma once

#include <optional>
#include <desyl/ast.hpp>
#include <desyl/desyllib.hpp>
#include <context.hpp>

namespace desyl
{
    /// @brief A fixed variable classification made at the start so variables don't become existential halfway through synthesis.
    struct VariableClassification
    {
        Vars ghosts;
        Vars existentials;

        VariableClassification() = default;
        VariableClassification(Vars const &precondition, Vars const &environment, Vars const &postcondition);
    };

    struct VariableSnapshot
    {
        Vars variables;
        Vars environment;
        VariableClassification classification;

        Vars all() const;
        Vars ghosts() const;
        Vars existentials() const;
    };

    class Goal
    {
        VariableClassification classification;

    public:
        Goal(FunctionSpecification spec, std::unordered_map<Identifier, FunctionSpecification> functions, std::unordered_map<Identifier, Predicate> predicates);
        FunctionSpecification spec;
        std::unordered_map<Identifier, FunctionSpecification> functions;
        std::unordered_map<Identifier, Predicate> predicates;
        Vars environment;
        VariableSnapshot variables() const;
    };

    void synthesize_query(Goal const &query, SynthesisMode mode);
}
