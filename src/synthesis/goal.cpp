#include <synthesis.hpp>

#include <algorithm>
#include <iostream>

namespace desyl
{
    VariableClassification::VariableClassification(Vars const &precondition, Vars const &environment, Vars const &postcondition)
    {
        Vars logic_variables;
        std::set_union(
            precondition.begin(), precondition.end(),
            postcondition.begin(), postcondition.end(),
            std::inserter(logic_variables, logic_variables.end()));
        std::set_difference(
            logic_variables.begin(), logic_variables.end(),
            environment.begin(), environment.end(),
            std::inserter(ghosts, ghosts.end()));
        std::set_difference(
            ghosts.begin(), ghosts.end(),
            precondition.begin(), precondition.end(),
            std::inserter(existentials, existentials.end()));
    }

    Goal::Goal(FunctionSpecification spec, std::unordered_map<Identifier, FunctionSpecification> functions, std::unordered_map<Identifier, Predicate> predicates, Vars additional_environment)
    {
        environment.insert(additional_environment.begin(), additional_environment.end());
        vars(spec.signature, environment);
        Vars precondition, postcondition;
        vars(spec.precondition, precondition);
        vars(spec.postcondition, postcondition);
        this->spec = std::move(spec);
        this->functions = std::move(functions);
        this->predicates = std::move(predicates);
        this->classification = VariableClassification(precondition, environment, postcondition);
    }

    Goal Goal::with_spec(FunctionSpecification spec) const
    {
        Goal result(std::move(spec), functions, predicates, environment);
        return result;
    }

    VariableSnapshot Goal::variables() const
    {
        Vars all;
        all.insert(environment.begin(), environment.end());
        vars(spec.precondition, all);
        vars(spec.postcondition, all);
        return VariableSnapshot{
            .variables = std::move(all),
            .environment = environment,
            .classification = classification,
        };
    }

    Vars VariableSnapshot::all() const
    {
        return variables;
    }

    Vars VariableSnapshot::ghosts() const
    {
        Vars result;
        std::set_intersection(
            variables.begin(), variables.end(),
            classification.ghosts.begin(), classification.ghosts.end(),
            std::inserter(result, result.end()));
        return result;
    }

    Vars VariableSnapshot::existentials() const
    {
        Vars result;
        std::set_intersection(
            variables.begin(), variables.end(),
            classification.existentials.begin(), classification.existentials.end(),
            std::inserter(result, result.end()));
        return result;
    }

    Identifier VariableSnapshot::rename_var(Identifier const &var) const
    {
        Vars all_vars = all();
        for (auto const &var : classification.ghosts)
        {
            all_vars.emplace(var);
        }
        for (auto const &var : classification.existentials)
        {
            all_vars.emplace(var);
        }
        return rename(var, all_vars);
    }
}
