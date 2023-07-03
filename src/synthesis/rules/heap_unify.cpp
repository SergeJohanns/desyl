#include <heap_unify.hpp>

#include <generic_continuations.hpp>
#include <substitution.hpp>
#include <overload.hpp>
#include <variant>
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace desyl
{
    std::optional<Substitutions> unify(Expression const &domain, Expression const &codomain, Vars const &variables)
    {
        if (domain == codomain)
        {
            Substitutions out;
            return out;
        }
        else if (std::holds_alternative<Identifier>(domain))
        {
            if (std::find(variables.begin(), variables.end(), std::get<Identifier>(domain)) != variables.end())
            {
                return std::optional<Substitutions>{{{std::get<Identifier>(domain), codomain}}};
            }
        }
        else if (std::holds_alternative<UnaryOperatorCall>(domain) && std::holds_alternative<UnaryOperatorCall>(codomain))
        {
            auto const &domain_call = std::get<UnaryOperatorCall>(domain);
            auto const &codomain_call = std::get<UnaryOperatorCall>(codomain);

            if (domain_call.type == codomain_call.type)
            {
                auto const &domain_operand = domain_call.operand;
                auto const &codomain_operand = codomain_call.operand;
                return unify(*domain_operand, *codomain_operand, variables);
            }
        }
        else if (std::holds_alternative<BinaryOperatorCall>(domain) && std::holds_alternative<BinaryOperatorCall>(codomain))
        {
            auto const &domain_call = std::get<BinaryOperatorCall>(domain);
            auto const &codomain_call = std::get<BinaryOperatorCall>(codomain);

            if (domain_call.type == codomain_call.type)
            {
                auto const &domain_lhs = domain_call.lhs;
                auto const &domain_rhs = domain_call.rhs;
                auto const &codomain_lhs = codomain_call.lhs;
                auto const &codomain_rhs = codomain_call.rhs;
                auto const &unified_lhs = unify(*domain_lhs, *codomain_lhs, variables);
                if (!unified_lhs)
                {
                    return {};
                }
                auto const &unified_rhs = unify(*domain_rhs, *codomain_rhs, variables);
                if (!unified_rhs || subtitutions_conflict(*unified_lhs, *unified_rhs))
                {
                    return {};
                }
                Substitutions unified;
                unified.insert(unified_lhs->begin(), unified_lhs->end());
                unified.insert(unified_rhs->begin(), unified_rhs->end());
                return unified;
            }
        }
        return {};
    }

    void add_pointer_substitutions(Goal const &goal, std::vector<Substitutions> &substitutions)
    {
        auto const &existentials = goal.variables().existentials();
        for (auto const &precondition : goal.spec.precondition.heap.pointer_declarations)
        {
            for (auto const &postcondition : goal.spec.postcondition.heap.pointer_declarations)
            {
                auto const &unified_base = unify(*postcondition.base, *precondition.base, existentials);
                if (!unified_base)
                {
                    continue;
                }
                auto const &unified_value = unify(*postcondition.value, *precondition.value, existentials);
                if (!unified_value || subtitutions_conflict(*unified_base, *unified_value))
                {
                    continue;
                }
                Substitutions unified;
                unified.insert(unified_base->begin(), unified_base->end());
                unified.insert(unified_value->begin(), unified_value->end());
                // If both are trivially equal, then we don't need to add the substitution
                if (unified.size() != 0)
                {
                    substitutions.push_back(unified);
                }
            }
        }
    }

    void add_predicate_substitutions(Goal const &, std::vector<Substitutions> &)
    {
        // TODO
    }

    std::vector<Derivation> HeapUnifyRule::apply(Goal const &goal) const
    {
        std::vector<Substitutions> substitutions;
        add_pointer_substitutions(goal, substitutions);
        add_predicate_substitutions(goal, substitutions);
        if (substitutions.size() > 0)
        {
            std::cout << "Using HEAPUNIFY" << std::endl;
        }
        std::vector<Derivation> derivations;
        for (auto const &substitution : substitutions)
        {
            derivations.push_back(Derivation{
                .goals = std::vector<Goal>{substitute(goal, substitution)},
                .continuation = std::make_unique<IdentityContinuation>(IdentityContinuation()),
            });
        }
        return derivations;
    }
}
