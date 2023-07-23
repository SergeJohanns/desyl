#include <branch.hpp>

#include <sstream>
#include <algorithm>

namespace desyl
{
    Program BranchContinuation::join(std::vector<Program> const &results) const
    {
        std::ostringstream stream;
        stream << "if (" << stringify_expression(condition) << ") ";
        stream << results[0];
        stream << " else ";
        stream << results[1];
        Program program("");
        return program.add_lines(stream.str());
    }

    std::vector<Derivation> BranchRule::apply(Goal const &goal) const
    {
        std::vector<Derivation> derivations;
        for (auto const &clause : learned_clauses)
        {
            if (std::find(goal.spec.precondition.proposition.begin(), goal.spec.precondition.proposition.end(), clause) != goal.spec.precondition.proposition.end())
            {
                continue;
            }
            Expression negated_clause = UnaryOperatorCall{
                .type = UnaryOperator::Not,
                .operand = std::make_shared<Expression>(clause),
            };
            if (std::find(goal.spec.precondition.proposition.begin(), goal.spec.precondition.proposition.end(), negated_clause) != goal.spec.precondition.proposition.end())
            {
                continue;
            }
            Goal ifgoal(goal);
            ifgoal.spec.precondition.proposition.push_back(clause);
            Goal elsegoal(goal);
            elsegoal.spec.precondition.proposition.push_back(negated_clause);
            derivations.push_back(Derivation{
                .goals = {ifgoal, elsegoal},
                .continuation = std::make_unique<BranchContinuation>(clause),
            });
        }
        return derivations;
    }
}
