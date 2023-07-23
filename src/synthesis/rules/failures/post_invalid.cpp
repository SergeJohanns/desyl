#include <post_invalid.hpp>

#include <iostream>
#include <algorithm>

namespace desyl
{
    std::vector<Derivation>
    PostInvalidRule::apply(Goal const &goal) const
    {
        if (goal.spec.postcondition.proposition.size() == 1 &&
            std::find(learned_clauses.begin(), learned_clauses.end(), goal.spec.postcondition.proposition[0]) == learned_clauses.end())
        {
            learned_clauses.push_back(goal.spec.postcondition.proposition[0]);
        }
        return {};
    }
}
