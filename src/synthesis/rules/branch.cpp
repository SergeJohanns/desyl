#include <branch.hpp>

#include <sstream>
#include <algorithm>

namespace desyl
{
    Program BranchContinuation::join(std::vector<Program> const &results) const
    {
        std::ostringstream stream;
        stream << "if (" << stringify_expression(condition) << ")" << std::endl;
        stream << results[0];
        stream << "else" << std::endl;
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
            std::cout << "Using BRANCH" << std::endl;
            Goal ifgoal(goal);
            ifgoal.spec.precondition.proposition.push_back(clause);
            for (auto &postcondition : ifgoal.spec.precondition.proposition)
            {
                std::cout << "IF: " << stringify_expression(postcondition) << std::endl;
            }
            for (auto &postcondition : ifgoal.spec.postcondition.proposition)
            {
                std::cout << "IF POST: " << stringify_expression(postcondition) << std::endl;
            }
            Goal elsegoal(goal);
            elsegoal.spec.precondition.proposition.push_back(negated_clause);
            for (auto &postcondition : elsegoal.spec.precondition.proposition)
            {
                std::cout << "ELSE: " << stringify_expression(postcondition) << std::endl;
            }
            for (auto &postcondition : elsegoal.spec.postcondition.proposition)
            {
                std::cout << "ELSE POST: " << stringify_expression(postcondition) << std::endl;
            }
            std::cout << (clause == negated_clause) << std::endl;
            derivations.push_back(Derivation{
                .goals = {ifgoal, elsegoal},
                .continuation = std::make_unique<BranchContinuation>(clause),
            });
        }
        return derivations;
    }
}
