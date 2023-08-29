#include <write.hpp>

#include <iostream>
#include <algorithm>
#include <sstream>

namespace desyl
{
    WriteContinuation::WriteContinuation(PointerDeclaration const &pointer)
    {
        std::ostringstream stream;
        stream << "*(" << stringify_expression(*pointer.base) << " + " << pointer.offset << ") = " << stringify_expression(*pointer.value) << ";" << std::endl;
        line = stream.str();
    }

    Program WriteContinuation::join(std::vector<Program> const &result) const
    {
        return result[0].add_lines(line);
    }

    std::vector<Derivation> WriteRule::apply(Goal const &goal) const
    {
        std::vector<Derivation> result;
        auto &preconditions = goal.spec.precondition.heap.pointer_declarations;
        auto &postconditions = goal.spec.postcondition.heap.pointer_declarations;
        for (size_t i = 0; i < preconditions.size(); ++i)
        {
            for (size_t j = 0; j < postconditions.size(); ++j)
            {
                if (*preconditions[i].base == *postconditions[j].base && preconditions[i].offset == postconditions[j].offset && *preconditions[i].value != *postconditions[j].value)
                {
                    Vars term_vars;
                    auto environment = goal.variables().environment;
                    vars(*postconditions[j].value, term_vars);
                    if (!std::includes(
                            environment.begin(), environment.end(),
                            term_vars.begin(), term_vars.end()))
                    {
                        continue;
                    }
                    auto new_spec(goal.spec);
                    auto new_declaration = PointerDeclaration{
                        .base = preconditions[i].base,
                        .offset = preconditions[i].offset,
                        .value = postconditions[j].value,
                    };
                    new_spec.precondition.heap.pointer_declarations[i] = new_declaration;
                    Derivation deriv{
                        .goals = std::vector<Goal>{goal.with_spec(new_spec)},
                        .continuation = std::make_unique<WriteContinuation>(WriteContinuation(new_declaration)),
                    };
                    result.push_back(std::move(deriv));
                }
            }
        }
        return result;
    }
}
