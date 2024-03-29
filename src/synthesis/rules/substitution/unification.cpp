#include <unification.hpp>

#include <algorithm>

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
        else if (std::holds_alternative<SetLiteral>(domain) && std::holds_alternative<SetLiteral>(codomain))
        {
            auto const &domain_set = std::get<SetLiteral>(domain);
            auto const &codomain_set = std::get<SetLiteral>(codomain);
            if (domain_set.elements.size() != codomain_set.elements.size())
            {
                return {};
            }

            Substitutions unified;
            for (size_t i = 0; i < domain_set.elements.size(); i++)
            {
                auto const &unified_element = unify(domain_set.elements[i], codomain_set.elements[i], variables);
                if (!unified_element || subtitutions_conflict(unified, *unified_element))
                {
                    return {};
                }
                unified.insert(unified_element->begin(), unified_element->end());
            }
            return unified;
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

    std::optional<Substitutions> unify(PointerDeclaration const &domain, PointerDeclaration const &codomain, Vars const &variables)
    {
        auto const &unified_base = unify(*domain.base, *codomain.base, variables);
        if (!unified_base)
        {
            return {};
        }
        auto const &unified_value = unify(*domain.value, *codomain.value, variables);
        if (!unified_value || subtitutions_conflict(*unified_base, *unified_value))
        {
            return {};
        }
        Substitutions unified;
        unified.insert(unified_base->begin(), unified_base->end());
        unified.insert(unified_value->begin(), unified_value->end());
        return unified;
    }

    std::optional<Substitutions> unify(PredicateCall const &domain, PredicateCall const &codomain, Vars const &variables)
    {
        if (domain.name != codomain.name)
        {
            return {};
        }
        if (domain.args.size() != codomain.args.size())
        {
            return {};
        }
        Substitutions unified;
        for (size_t i = 0; i < domain.args.size(); i++)
        {
            auto const &unified_arg = unify(domain.args[i], codomain.args[i], variables);
            if (!unified_arg)
            {
                return {};
            }
            unified.insert(unified_arg->begin(), unified_arg->end());
        }
        return unified;
    }
}
