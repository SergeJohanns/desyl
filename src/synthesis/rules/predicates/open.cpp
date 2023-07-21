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
        std::ostringstream stream;
        stream << "if (" << stringify_expression(clauses[0].condition) << ")" << std::endl;
        stream << results[0];
        for (size_t i = 1; i < clauses.size() - 1; i++)
        {
            stream << "else if (" << stringify_expression(clauses[i].condition) << ")" << std::endl;
            stream << results[i];
        }
        stream << "else" << std::endl;
        stream << results[clauses.size() - 1];
        Program program("");
        return program.add_lines(stream.str());
    }

    bool all_program_variables(std::vector<Identifier> const &args, Vars const &program_variables)
    {
        for (auto const &arg : args)
        {
            if (program_variables.find(arg) == program_variables.end())
            {
                return false;
            }
        }
        return true;
    }

    std::vector<Derivation> OpenRule::apply(Goal const &goal) const
    {
        std::vector<Derivation> derivations;
        Vars const &environment = goal.environment;
        for (auto const &predicate_call : goal.spec.precondition.heap.predicate_calls)
        {
            const bool predicate_exists = goal.predicates.find(predicate_call.name) != goal.predicates.end();
            if (!predicate_exists || !all_program_variables(predicate_call.args, environment))
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
            std::vector<Goal> goals;
            for (auto const &clause : predicate.clauses)
            {
                Goal new_goal(goal);
                new_goal.spec.precondition.proposition.push_back(substitute(clause.condition, substitution));
                Proposition consequent = substitute(clause.assertion.proposition, substitution);
                new_goal.spec.precondition.proposition.insert(new_goal.spec.precondition.proposition.end(), consequent.begin(), consequent.end());
                Heap heap = substitute(clause.assertion.heap, substitution);
                new_goal.spec.precondition.heap.pointer_declarations.insert(new_goal.spec.precondition.heap.pointer_declarations.end(), heap.pointer_declarations.begin(), heap.pointer_declarations.end());
                new_goal.spec.precondition.heap.array_declarations.insert(new_goal.spec.precondition.heap.array_declarations.end(), heap.array_declarations.begin(), heap.array_declarations.end());
                new_goal.spec.precondition.heap.predicate_calls.insert(new_goal.spec.precondition.heap.predicate_calls.end(), heap.predicate_calls.begin(), heap.predicate_calls.end());
                goals.push_back(new_goal);
            }
            derivations.push_back(Derivation{
                .goals = std::move(goals),
                .continuation = std::make_unique<OpenContinuation>(OpenContinuation{predicate.clauses}),
            });
        }
        if (!derivations.empty())
        {
            std::cout << "Using OPEN" << std::endl;
        }
        return derivations;
    }
}
