#include <emp.hpp>

#include <generic_continuations/generic_continuations.hpp>
#include <iostream>

namespace desyl
{
    std::vector<Derivation> EmpRule::apply(Goal const &goal) const
    {
        auto const &pre = std::move(goal).spec.precondition;
        auto const &precondition_empty = pre.heap.pointer_declarations.size() == 0 && pre.heap.predicate_calls.size() == 0 && pre.heap.array_declarations.size() == 0;
        auto const &post = std::move(goal).spec.postcondition;
        auto const &postcondition_empty = post.heap.pointer_declarations.size() == 0 && post.heap.predicate_calls.size() == 0 && post.heap.array_declarations.size() == 0;
        auto const &proposition_tautology = post.proposition.size() == 0;
        if (precondition_empty && postcondition_empty && proposition_tautology)
        {
            std::cout << "Using EMP" << std::endl;
            auto deriv = Derivation{
                .goals = std::vector<Goal>{},
                .continuation = std::make_unique<EmptyContinuation>(std::move(EmptyContinuation{})),
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
