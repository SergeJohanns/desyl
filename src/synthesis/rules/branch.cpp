#include <branch.hpp>

#include <sstream>
#include <algorithm>

namespace desyl
{
    Program BranchContinuation::join(std::vector<Program> const &results) const
    {
        std::ostringstream stream;
        stream << results[0];
        stream << results[1];
        return Program(stream.str());
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
            std::cout << "Using BRANCH" << std::endl;
            Goal ifgoal(goal);
            ifgoal.spec.precondition.proposition.push_back(clause);
            Goal elsegoal(goal);
            elsegoal.spec.precondition.proposition.push_back(UnaryOperatorCall{
                .type = UnaryOperator::Not,
                .operand = std::make_shared<Expression>(clause),
            });
            derivations.push_back(Derivation{
                .goals = {ifgoal, elsegoal},
                .continuation = std::make_unique<BranchContinuation>(clause),
            });
        }
        return derivations;
    }
}
