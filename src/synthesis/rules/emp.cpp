#include <emp.hpp>

#include <generic_continuations.hpp>
#include <iostream>

namespace desyl
{
    std::vector<Derivation> EmpRule::apply(Goal const &goal) const
    {
        auto const &precondition_empty = std::move(goal).spec.precondition.heap.pointer_declarations.size() == 0;
        auto const &postcondition_empty = std::move(goal).spec.postcondition.heap.pointer_declarations.size() == 0;
        if (precondition_empty && postcondition_empty)
        {
            std::cout << "Using EMP" << std::endl;
            auto deriv = Derivation{
                .goals = std::vector<Goal>{},
                .continuation = std::make_unique<ConstantContinuation>(ConstantContinuation("emp;\n")),
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
