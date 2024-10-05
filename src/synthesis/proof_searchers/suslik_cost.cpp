#include <suslik_cost.hpp>

namespace desyl
{
    int const PRECONDITION_COST_FACTOR = 3;
    int const PREDICATE_COST_FACTOR = 2;

    int suslik_cost(Assertion const &assertion)
    {
        int cost = 0;
        for (auto const &pred : assertion.heap.predicate_calls)
        {
            cost += PREDICATE_COST_FACTOR * (pred.label + 1);
        }
        cost += assertion.heap.array_declarations.size();
        cost += assertion.heap.pointer_declarations.size();
        return cost;
    }

    int suslik_cost(ProofTreeNode const &node)
    {
        auto spec = node.goal->spec;
        return PRECONDITION_COST_FACTOR * suslik_cost(spec.precondition) + suslik_cost(spec.postcondition);
    }
}
