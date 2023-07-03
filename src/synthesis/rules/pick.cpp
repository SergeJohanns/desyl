#include <pick.hpp>

#include <substitution.hpp>
#include <generic_continuations.hpp>
#include <iostream>

namespace desyl
{
    bool duplicate_pointer_base(std::vector<PointerDeclaration> const &pointers)
    {
        std::cout << "Woo " << pointers.size() << std::endl;
        for (size_t i = 0; i < pointers.size(); i++)
        {
            for (size_t j = i + 1; j < pointers.size(); j++)
            {
                std::cout << "(" << i << ", " << j << "): " << stringify_expression(*pointers[i].base) << " " << stringify_expression(*pointers[j].base) << std::endl;
                if (pointers[i].base == pointers[j].base)
                {
                    std::cout << "Duplicate pointer base: " << stringify_expression(*pointers[i].base) << std::endl;
                    return true;
                }
            }
        }
        return false;
    }

    std::vector<Derivation> PickRule::apply(Goal const &goal) const
    {
        std::vector<Derivation> derivations;
        auto const &variables = goal.variables();
        for (auto const &existential : variables.existentials())
        {
            std::cout << "Existential: " << existential << std::endl;
        }
        for (auto const &existential : variables.existentials())
        {
            for (auto const &program : variables.environment)
            {
                Substitutions substitution = {{existential, program}};
                std::cout << "Substitution: " << existential << " := " << program << std::endl;
                Goal new_goal = substitute(goal, substitution);
                if (duplicate_pointer_base(new_goal.spec.precondition.heap.pointer_declarations) ||
                    duplicate_pointer_base(new_goal.spec.postcondition.heap.pointer_declarations))
                {
                    continue;
                }
                derivations.push_back(Derivation{
                    .goals = std::vector<Goal>{new_goal},
                    .continuation = std::make_unique<IdentityContinuation>(IdentityContinuation()),
                });
            }
        }
        if (derivations.size() > 0)
        {
            std::cout << "Applying PICK" << std::endl;
            quick_exit(0);
        }
        return derivations;
    }
}
