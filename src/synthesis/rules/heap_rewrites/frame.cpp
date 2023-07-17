#include <frame.hpp>

#include <generic_continuations/generic_continuations.hpp>
#include <iostream>
#include <algorithm>
#include <functional>

namespace desyl
{
    template <typename HeapElement>
    void add_derivations(std::vector<Derivation> &derivations, Goal const &goal, std::vector<HeapElement> const &precondition, std::vector<HeapElement> const &postcondition, std::function<std::vector<HeapElement> &(Heap &)> const &getter)
    {
        for (size_t i = 0; i < precondition.size(); ++i)
        {
            for (size_t j = 0; j < postcondition.size(); ++j)
            {
                if (precondition[i] == postcondition[j])
                {
                    Vars term, intersection;
                    vars(precondition[i], term);

                    auto new_goal(goal);
                    auto &new_precondition = getter(new_goal.spec.precondition.heap);
                    auto &new_postcondition = getter(new_goal.spec.postcondition.heap);
                    new_precondition.erase(new_precondition.begin() + i);
                    new_postcondition.erase(new_postcondition.begin() + j);

                    auto const &existentials = new_goal.variables().existentials();
                    std::set_intersection(term.begin(), term.end(), existentials.begin(), existentials.end(), std::inserter(intersection, intersection.end()));
                    if (intersection.size() == 0)
                    {
                        derivations.push_back(Derivation{
                            .goals = {new_goal},
                            .continuation = std::make_unique<IdentityContinuation>(),
                        });
                    }
                }
            }
        }
    }

    std::vector<Derivation> FrameRule::apply(Goal const &goal) const
    {
        std::vector<Derivation> derivations;

        auto const &precondition_pointers = goal.spec.precondition.heap.pointer_declarations;
        auto const &postcondition_pointers = goal.spec.postcondition.heap.pointer_declarations;
        add_derivations<PointerDeclaration>(derivations, goal, precondition_pointers, postcondition_pointers,
                                            [](Heap &heap) -> std::vector<PointerDeclaration> &
                                            { return heap.pointer_declarations; });

        auto const &precondition_arrays = goal.spec.precondition.heap.array_declarations;
        auto const &postcondition_arrays = goal.spec.postcondition.heap.array_declarations;
        add_derivations<ArrayDeclaration>(derivations, goal, precondition_arrays, postcondition_arrays,
                                          [](Heap &heap) -> std::vector<ArrayDeclaration> &
                                          { return heap.array_declarations; });

        auto const &precondition_predicates = goal.spec.precondition.heap.predicate_calls;
        auto const &postcondition_predicates = goal.spec.postcondition.heap.predicate_calls;
        add_derivations<PredicateCall>(derivations, goal, precondition_predicates, postcondition_predicates,
                                       [](Heap &heap) -> std::vector<PredicateCall> &
                                       { return heap.predicate_calls; });

        if (derivations.size() > 0)
        {
            std::cout << "Using FRAME" << std::endl;
        }

        return derivations;
    }
}
