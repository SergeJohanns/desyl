#include <free.hpp>

#include <iostream>
#include <algorithm>
#include <sstream>

namespace desyl
{
    FreeContinuation::FreeContinuation(Expression const &location)
    {
        std::ostringstream stream;
        stream << "free(" << stringify_expression(location) << ");" << std::endl;
        line = stream.str();
    }

    Program FreeContinuation::join(std::vector<Program> const &result) const
    {
        return result[0].add_lines(line);
    }

    bool unmatched_block(ArrayDeclaration const &block, Goal const &goal)
    {
        for (auto const &postcondition_block : goal.spec.postcondition.heap.array_declarations)
        {
            if (block == postcondition_block)
            {
                return false;
            }
        }
        return true;
    }

    int pointer_index(ArrayDeclaration const &block, int i, Goal const &goal)
    {

        for (size_t j = 0; j < goal.spec.precondition.heap.pointer_declarations.size(); ++j)
        {
            auto const &pointer = goal.spec.precondition.heap.pointer_declarations[j];
            if (!std::holds_alternative<Identifier>(*pointer.base))
            {
                continue;
            }
            const auto &identifier = std::get<Identifier>(*pointer.base);
            if (identifier == block.name && pointer.offset == i)
            {
                return j;
            }
        }
        return -1;
    }

    std::vector<Derivation> FreeRule::apply(Goal const &goal) const
    {
        std::vector<Derivation> derivations;
        for (size_t i = 0; i < goal.spec.precondition.heap.array_declarations.size(); ++i)
        {
            auto const &block = goal.spec.precondition.heap.array_declarations[i];
            bool environment = goal.environment.find(block.name) != goal.environment.end();
            if (!environment || !unmatched_block(block, goal))
            {
                continue;
            }
            std::vector<int> deletions;
            for (int j = 0; j < block.size; ++j)
            {
                int index = pointer_index(block, j, goal);
                if (index != -1)
                {
                    deletions.push_back(index);
                }
            }
            if ((int)deletions.size() != block.size)
            {
                continue;
            }
            std::sort(deletions.begin(), deletions.end(), std::greater<int>());

            auto new_goal(goal);
            new_goal.spec.precondition.heap.array_declarations.erase(new_goal.spec.precondition.heap.array_declarations.begin() + i);
            for (auto const &index : deletions)
            {
                new_goal.spec.precondition.heap.pointer_declarations.erase(new_goal.spec.precondition.heap.pointer_declarations.begin() + index);
            }
            derivations.push_back(Derivation{
                .goals = std::vector<Goal>{new_goal},
                .continuation = std::make_unique<FreeContinuation>(FreeContinuation(block.name)),
            });
        }
        if (!derivations.empty())
        {
            std::cout << "Using FREE" << std::endl;
        }
        return derivations;
    }
}
