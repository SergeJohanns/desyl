#include <rules.hpp>

#include <vector>
#include <memory>

namespace desyl
{
    Goal::Goal(FunctionSpecification spec)
    {
        this->spec = std::move(spec);
        vars(spec.signature, environment);
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
