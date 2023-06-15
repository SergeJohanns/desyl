#include <rules.hpp>

#include <vector>
#include <memory>
#include <algorithm>

namespace desyl
{
    Goal::Goal(FunctionSpecification spec)
    {
        this->spec = std::move(spec);
        vars(spec.signature, environment);
    }

    VariableSnapshot Goal::variables()
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

    Vars VariableSnapshot::all()
    {
        Vars result;
        result.insert(precondition.begin(), precondition.end());
        result.insert(environment.begin(), environment.end());
        result.insert(postcondition.begin(), postcondition.end());
        return result;
    }

    Vars VariableSnapshot::ghosts()
    {
        Vars result;
        std::set_difference(
            precondition.begin(), precondition.end(),
            environment.begin(), environment.end(),
            std::inserter(result, result.end()));
        return result;
    }

    Vars VariableSnapshot::existentials()
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

    Program ConstantContinuation::join(std::vector<Program> const &) const
    {
        return program;
    }

    std::vector<Derivation> EmpRule::apply(Goal const &goal) const
    {
        auto const &precondition_empty = std::move(goal).spec.precondition.heap.pointer_declarations.size() == 0;
        auto const &postcondition_empty = std::move(goal).spec.postcondition.heap.pointer_declarations.size() == 0;
        if (precondition_empty && postcondition_empty)
        {
            auto deriv = Derivation{
                .goals = std::vector<Goal>{},
                .continuation = std::make_unique<ConstantContinuation>(ConstantContinuation("emp")),
            };
            std::vector<Derivation> result;
            result.push_back(std::move(deriv));
            return result;
        }
        else
        {
            return std::vector<Derivation>{};
        }
    }
}
