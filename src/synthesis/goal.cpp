#include <synthesis.hpp>

#include <algorithm>

namespace desyl
{
    Goal::Goal(FunctionSpecification spec)
    {
        vars(spec.signature, environment);
        this->spec = std::move(spec);
    }

    VariableSnapshot Goal::variables() const
    {
        Vars precondition, postcondition;
        vars(spec.precondition, precondition);
        vars(spec.postcondition, postcondition);
        return VariableSnapshot{
            .precondition = std::move(precondition),
            .environment = std::move(environment),
            .postcondition = std::move(postcondition),
        };
    }

    Vars VariableSnapshot::all() const
    {
        Vars result;
        result.insert(precondition.begin(), precondition.end());
        result.insert(environment.begin(), environment.end());
        result.insert(postcondition.begin(), postcondition.end());
        return result;
    }

    Vars VariableSnapshot::ghosts() const
    {
        Vars result;
        std::set_difference(
            precondition.begin(), precondition.end(),
            environment.begin(), environment.end(),
            std::inserter(result, result.end()));
        return result;
    }

    Vars VariableSnapshot::existentials() const
    {
        // We need to union the signature and precondition separately because reusing the result
        // as both input and output is UB
        Vars exclusion, result;
        std::set_union(
            precondition.begin(), precondition.end(),
            environment.begin(), environment.end(),
            std::inserter(exclusion, exclusion.end()));
        std::set_difference(
            postcondition.begin(), postcondition.end(),
            exclusion.begin(), exclusion.end(),
            std::inserter(result, result.end()));
        return result;
    }
}
