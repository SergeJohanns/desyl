#include <alloc.hpp>

#include <substitution/substitution.hpp>
#include <iostream>
#include <algorithm>
#include <sstream>

namespace desyl
{
    AllocContinuation::AllocContinuation(Identifier const &output, Literal const &size)
    {
        std::ostringstream stream;
        stream << output << " = malloc(" << size << ");" << std::endl;
        line = stream.str();
    }

    Program AllocContinuation::join(std::vector<Program> const &result) const
    {
        return result[0].add_lines(line);
    }

    bool unmatched_pre_block(ArrayDeclaration const &block, Goal const &goal)
    {
        for (auto const &precondition_block : goal.spec.precondition.heap.array_declarations)
        {
            std::cout << "Comparing " << precondition_block.name << " and " << block.name << std::endl;
            // Unlike with the FreeRule, we keep in mind we can't allocate if the pointer already
            // has a block, even if it's not the same size, because of the separating conjunction
            if (block.name == precondition_block.name)
            {
                return false;
            }
        }
        return true;
    }

    std::vector<Derivation> AllocRule::apply(Goal const &goal) const
    {
        std::vector<Derivation> derivations;
        auto variables = goal.variables();
        auto const all = variables.all();
        auto const existentials = variables.existentials();
        for (size_t i = 0; i < goal.spec.postcondition.heap.array_declarations.size(); ++i)
        {
            auto const &block = goal.spec.postcondition.heap.array_declarations[i];
            bool existential = existentials.find(block.name) != existentials.end();
            if (!existential || !unmatched_pre_block(block, goal))
            {
                continue;
            }

            auto const &identifier = block.name;
            Identifier new_var;
            for (int i = 0;; ++i)
            {
                new_var = identifier + "_" + std::to_string(i);
                if (all.find(new_var) == all.end())
                {
                    break;
                }
            }
            Substitutions substitutions = {
                {identifier, new_var},
            };

            auto new_goal(goal);
            new_goal.spec.postcondition = substitute(new_goal.spec.postcondition, substitutions);
            new_goal.environment.insert(new_var);
            new_goal.spec.precondition.heap.array_declarations.push_back(ArrayDeclaration{
                .name = new_var,
                .size = block.size,
            });
            for (int i = 0; i < block.size; i++)
            {
                new_goal.spec.precondition.heap.pointer_declarations.push_back(PointerDeclaration{
                    .base = std::make_shared<Expression>(new_var),
                    .offset = i,
                    .value = std::make_shared<Expression>(0),
                });
            }
            derivations.push_back(Derivation{
                .goals = std::vector<Goal>{new_goal},
                .continuation = std::make_unique<AllocContinuation>(AllocContinuation(new_var, block.size)),
            });
        }
        return derivations;
    }
}
