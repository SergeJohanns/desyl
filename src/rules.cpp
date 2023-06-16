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

    Program IdentityContinuation::join(std::vector<Program> const &result) const
    {
        return result[0];
    }

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
            precondition.erase(precondition.begin() + precondition_index);
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
        size_t array_size = precondition_pointers.size();
        remove_frame_overlap(existentials, precondition_arrays, postcondition_arrays);
        bool array_changed = array_size != precondition_arrays.size();

        auto &precondition_predicates = new_goal.spec.precondition.heap.predicate_calls;
        auto &postcondition_predicates = new_goal.spec.postcondition.heap.predicate_calls;
        size_t predicate_size = precondition_pointers.size();
        remove_frame_overlap(existentials, precondition_predicates, postcondition_predicates);
        bool predicate_changed = predicate_size != precondition_predicates.size();

        if (!pointer_changed && !array_changed && !predicate_changed)
        {
            return std::vector<Derivation>{};
        }

        Derivation deriv{
            .goals = std::vector<Goal>{new_goal},
            .continuation = std::make_unique<IdentityContinuation>(IdentityContinuation()),
        };
        std::vector<Derivation> result;
        result.push_back(std::move(deriv));
        return result;
    }
}
