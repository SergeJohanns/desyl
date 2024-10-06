#include <call.hpp>

#include <substitution/unification.hpp>
#include <algorithm>

namespace desyl
{
    Program CallContinuation::join(std::vector<Program> const &results) const
    {
        std::string call = function_name + "(";
        for (size_t i = 0; i < args.size(); i++)
        {
            call += stringify_expression(args[i]);
            if (i < args.size() - 1)
            {
                call += ", ";
            }
        }
        call += ");";
        return results[0].add_lines(call);
    }

    /// @brief Execute the effects of a subheap unification
    /// @param unification The unification to execute
    /// @return The derivation of the effects, if any
    std::optional<Derivation> execute_subheap_unification(SubHeapUnification const &unification, Goal const &goal, FunctionSpecification const &spec, Vars const &environment)
    {
        std::vector<Expression> args;
        Vars arg_variables;
        for (auto const &var : spec.signature.args)
        {
            args.push_back(substitute(var.name, unification.substitutions));
            vars(args.back(), arg_variables);
        }
        if (!std::includes(environment.begin(), environment.end(), arg_variables.begin(), arg_variables.end()))
        {
            return std::nullopt;
        }

        VariableSnapshot const &variables = goal.variables();
        Vars const &goal_variables = variables.all();
        Substitutions substitution = unification.substitutions;
        // Addtional check of environment because absence from the substitution can mean they have the same name on purpose
        for (auto const &var : spec.signature.args)
        {
            if (substitution.find(var.name) == substitution.end())
            {
                substitution[var.name] = var.name;
            }
        }
        Vars spec_variables;
        vars(spec.postcondition, spec_variables);
        for (auto const &variable : spec_variables)
        {
            if (substitution.find(variable) == substitution.end())
            {
                substitution[variable] = rename(variable, spec_variables);
            }
        }

        FunctionSpecification new_spec(goal.spec);
        std::vector<size_t> pointer_indices(unification.pointer_indices.begin(), unification.pointer_indices.end());
        std::sort(pointer_indices.begin(), pointer_indices.end(), std::greater<size_t>());
        for (auto const &index : pointer_indices)
        {
            new_spec.precondition.heap.pointer_declarations.erase(new_spec.precondition.heap.pointer_declarations.begin() + index);
        }
        std::vector<size_t> predicate_indices(unification.predicate_indices.begin(), unification.predicate_indices.end());
        std::sort(predicate_indices.begin(), predicate_indices.end(), std::greater<size_t>());
        for (auto const &index : predicate_indices)
        {
            new_spec.precondition.heap.predicate_calls.erase(new_spec.precondition.heap.predicate_calls.begin() + index);
        }
        auto const &consequent = substitute(spec.postcondition, substitution);
        new_spec.precondition.proposition.insert(new_spec.precondition.proposition.end(), consequent.proposition.begin(), consequent.proposition.end());
        new_spec.precondition.heap.pointer_declarations.insert(new_spec.precondition.heap.pointer_declarations.end(), consequent.heap.pointer_declarations.begin(), consequent.heap.pointer_declarations.end());
        new_spec.precondition.heap.predicate_calls.insert(new_spec.precondition.heap.predicate_calls.end(), consequent.heap.predicate_calls.begin(), consequent.heap.predicate_calls.end());

        return Derivation{
            .goals = std::vector<Goal>{goal.with_spec(std::move(new_spec))},
            .continuation = std::make_unique<CallContinuation>(spec.signature.name, args),
        };
    }

    std::vector<Derivation> CallRule::apply(Goal const &goal) const
    {
        std::vector<Derivation> derivations;
        for (auto const &[name, spec] : goal.functions)
        {
            // TODO: Support recursive calls to functions with nontrivial pure preconditions
            if (!spec.precondition.proposition.empty())
            {
                continue;
            }

            Vars variables;
            // Not just program variables but all variables in the precondition can be substituted
            vars(spec.precondition, variables);
            auto unifications = unify_subheaps(spec.precondition.heap, goal.spec.precondition.heap, variables);

            for (auto unification : unifications)
            {
                auto derivation = execute_subheap_unification(unification, goal, spec, goal.environment);
                if (derivation.has_value())
                {
                    derivations.push_back(std::move(derivation.value()));
                }
            }
        }
        return derivations;
    }
}
