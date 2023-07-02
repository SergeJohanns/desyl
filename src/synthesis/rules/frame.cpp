#include <frame.hpp>

#include <generic_continuations.hpp>
#include <iostream>
#include <algorithm>

namespace desyl
{
    template <typename HeapElement>
    void remove_frame_overlap(Vars const &existentials, std::vector<HeapElement> &precondition, std::vector<HeapElement> &postcondition)
    {
        std::vector<std::pair<size_t, size_t>> overlap;
        for (size_t i = 0; i < precondition.size(); ++i)
        {
            for (size_t j = 0; j < postcondition.size(); ++j)
            {
                if (precondition[i] == postcondition[j])
                {
                    overlap.push_back(std::make_pair(i, j));
                }
            }
        }
        std::vector<size_t> precondition_to_remove, postcondition_to_remove;
        for (auto [precondition_index, postcondition_index] : overlap)
        {
            Vars term_vars, intersection;
            vars(precondition[precondition_index], term_vars);
            std::set_intersection(
                term_vars.begin(), term_vars.end(),
                existentials.begin(), existentials.end(),
                std::inserter(intersection, intersection.end()));
            if (intersection.size() != 0)
            {
                continue;
            }
            precondition_to_remove.push_back(precondition_index);
            postcondition_to_remove.push_back(postcondition_index);
        }
        std::sort(precondition_to_remove.begin(), precondition_to_remove.end(), std::greater());
        std::sort(postcondition_to_remove.begin(), postcondition_to_remove.end(), std::greater());
        for (auto precondition_index : precondition_to_remove)
        {
            precondition.erase(precondition.begin() + precondition_index);
        }
        for (auto postcondition_index : postcondition_to_remove)
        {
            postcondition.erase(postcondition.begin() + postcondition_index);
        }
    }

    std::vector<Derivation> FrameRule::apply(Goal const &goal) const
    {
        auto new_goal(goal);
        auto const existentials = new_goal.variables().existentials();

        auto &precondition_pointers = new_goal.spec.precondition.heap.pointer_declarations;
        auto &postcondition_pointers = new_goal.spec.postcondition.heap.pointer_declarations;
        size_t pointer_size = precondition_pointers.size();
        remove_frame_overlap(existentials, precondition_pointers, postcondition_pointers);
        bool pointer_changed = pointer_size != precondition_pointers.size();

        auto &precondition_arrays = new_goal.spec.precondition.heap.array_declarations;
        auto &postcondition_arrays = new_goal.spec.postcondition.heap.array_declarations;
        size_t array_size = precondition_arrays.size();
        remove_frame_overlap(existentials, precondition_arrays, postcondition_arrays);
        bool array_changed = array_size != precondition_arrays.size();

        auto &precondition_predicates = new_goal.spec.precondition.heap.predicate_calls;
        auto &postcondition_predicates = new_goal.spec.postcondition.heap.predicate_calls;
        size_t predicate_size = precondition_predicates.size();
        remove_frame_overlap(existentials, precondition_predicates, postcondition_predicates);
        bool predicate_changed = predicate_size != precondition_predicates.size();

        if (!pointer_changed && !array_changed && !predicate_changed)
        {
            return std::vector<Derivation>{};
        }

        std::cout << "Using FRAME" << std::endl;
        Derivation deriv{
            .goals = std::vector<Goal>{new_goal},
            .continuation = std::make_unique<IdentityContinuation>(IdentityContinuation()),
        };
        std::vector<Derivation> result;
        result.push_back(std::move(deriv));
        return result;
    }
}
