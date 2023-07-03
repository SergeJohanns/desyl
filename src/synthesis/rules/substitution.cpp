#include <substitution.hpp>
#include <substitution.hpp>

namespace desyl
{
    void substitute_expression(Expression &expression, Identifier const &identifier, Expression const &substitution)
    {
        if (std::holds_alternative<Identifier>(expression) && std::get<Identifier>(expression) == identifier)
        {
            expression = substitution;
        }
        else if (std::holds_alternative<UnaryOperatorCall>(expression))
        {
            auto &call = std::get<UnaryOperatorCall>(expression);
            substitute_expression(*call.operand, identifier, substitution);
        }
        else if (std::holds_alternative<BinaryOperatorCall>(expression))
        {
            auto &call = std::get<BinaryOperatorCall>(expression);
            substitute_expression(*call.lhs, identifier, substitution);
            substitute_expression(*call.rhs, identifier, substitution);
        }
    }

    bool subtitutions_conflict(Substitutions const &first, Substitutions const &second)
    {
        for (auto const &[identifier, expression] : first)
        {
            if (second.find(identifier) != second.end() && second.at(identifier) != expression)
            {
                return true;
            }
        }
        return false;
    }

    Goal substitute(Goal const &goal, Substitutions const &substitutions)
    {
        Goal substituted(goal);
        for (auto const &[identifier, expression] : substitutions)
        {
            substitute_expression(substituted.spec.postcondition.proposition, identifier, expression);
            for (auto &pointer_declaration : substituted.spec.postcondition.heap.pointer_declarations)
            {
                substitute_expression(*pointer_declaration.base, identifier, expression);
                substitute_expression(*pointer_declaration.value, identifier, expression);
            }
        }
        return substituted;
    }
}
