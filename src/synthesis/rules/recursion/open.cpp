#include <open.hpp>

#include <substitution/substitution.hpp>
#include <sstream>
#include <algorithm>
#include <ranges>
#include <iostream>

namespace desyl
{
    Program OpenContinuation::join(std::vector<Program> const &results) const
    {
        if (clauses.size() == 1)
        {
            return results[0];
        }
        std::ostringstream stream;
        stream << "if (" << stringify_expression(clauses[0].condition) << ") ";
        stream << results[0];
        for (size_t i = 1; i < clauses.size() - 1; i++)
        {
            stream << " else if (" << stringify_expression(clauses[i].condition) << ") ";
            stream << results[i];
        }
        stream << " else ";
        stream << results[clauses.size() - 1];
        Program program("");
        return program.add_lines(stream.str());
    }

    bool all_program_variables(Substitutions const &substitutions, std::vector<Clause> const &clauses, Vars const &program_variables)
    {
        for (auto const &clause : clauses)
        {
            Vars clause_variables;
            vars(clause.condition, clause_variables);
            for (auto const &variable : clause_variables)
            {
                if (substitutions.find(variable) == substitutions.end())
                {
                    continue;
                }
                Vars variable_variables, difference;
                vars(substitutions.at(variable), variable_variables);
                std::set_difference(variable_variables.begin(), variable_variables.end(), program_variables.begin(), program_variables.end(), std::inserter(difference, difference.begin()));
                if (difference.size() > 0)
                {
                    return false;
                }
            }
        }
        return true;
    }

    std::vector<Derivation> OpenRule::apply(Goal const &goal) const
    {
        std::vector<Derivation> derivations;
        Vars const &environment = goal.environment;
        VariableClassification old_classification = goal.variables().classification;
        for (size_t i = 0; i < goal.spec.precondition.heap.predicate_calls.size(); ++i)
        {
            auto const &predicate_call = goal.spec.precondition.heap.predicate_calls[i];
            const bool predicate_exists = goal.predicates.find(predicate_call.name) != goal.predicates.end();
            if (predicate_call.label >= MAX_UNFOLDS || !predicate_exists)
            {
                continue;
            }
            auto const &predicate = goal.predicates.at(predicate_call.name);
            if (predicate.args.size() != predicate_call.args.size())
            {
                continue;
            }
            Substitutions substitution;
            for (size_t i = 0; i < predicate.args.size(); i++)
            {
                substitution[predicate.args[i].name] = predicate_call.args[i];
            }
            if (!all_program_variables(substitution, predicate.clauses, environment))
            {
                continue;
            }
            std::vector<Goal> goals;
            for (auto const &clause : predicate.clauses)
            {
                FunctionSpecification new_spec = goal.spec;

                new_spec.precondition.proposition.push_back(substitute(clause.condition, substitution));
                Proposition consequent = substitute(clause.assertion.proposition, substitution);
                new_spec.precondition.proposition.insert(new_spec.precondition.proposition.end(), consequent.begin(), consequent.end());

                Heap heap = substitute(clause.assertion.heap, substitution);
                new_spec.precondition.heap.pointer_declarations.insert(new_spec.precondition.heap.pointer_declarations.end(), heap.pointer_declarations.begin(), heap.pointer_declarations.end());
                new_spec.precondition.heap.array_declarations.insert(new_spec.precondition.heap.array_declarations.end(), heap.array_declarations.begin(), heap.array_declarations.end());
                new_spec.precondition.heap.predicate_calls.insert(new_spec.precondition.heap.predicate_calls.end(), heap.predicate_calls.begin(), heap.predicate_calls.end());
                for (auto &predicate_call : new_spec.precondition.heap.predicate_calls)
                {
                    predicate_call.label++;
                }

                new_spec.precondition.heap.predicate_calls.erase(new_spec.precondition.heap.predicate_calls.begin() + i);
                Goal new_goal(std::move(new_spec), goal.functions, goal.predicates);
                VariableClassification new_classification = new_goal.variables().classification;
                Vars difference;

                // Make sure that the old classification is a subset of the new classification
                std::set_difference(new_classification.ghosts.begin(), new_classification.ghosts.end(),
                                    old_classification.existentials.begin(), old_classification.existentials.end(),
                                    std::inserter(difference, difference.begin()));
                std::set_difference(difference.begin(), difference.end(),
                                    environment.begin(), environment.end(),
                                    std::inserter(new_classification.ghosts, new_classification.ghosts.begin()));
                std::set_difference(new_classification.existentials.begin(), new_classification.existentials.end(),
                                    old_classification.ghosts.begin(), old_classification.ghosts.end(),
                                    std::inserter(difference, difference.begin()));
                std::set_difference(difference.begin(), difference.end(),
                                    environment.begin(), environment.end(),
                                    std::inserter(new_classification.existentials, new_classification.existentials.begin()));

                goals.push_back(new_goal);
            }
            derivations.push_back(Derivation{
                .goals = std::move(goals),
                .continuation = std::make_unique<OpenContinuation>(OpenContinuation{predicate.clauses}),
            });
        }
        return derivations;
    }
}
