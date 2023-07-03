#include <substitution.hpp>

namespace desyl
{
    Expression substitute_expression(Expression const &expression, Identifier const &identifier, Expression const &substitution)
    {
        if (std::holds_alternative<Identifier>(expression) && std::get<Identifier>(expression) == identifier)
        {
            return substitution;
        }
        else if (std::holds_alternative<UnaryOperatorCall>(expression))
        {
            auto &call = std::get<UnaryOperatorCall>(expression);
            return UnaryOperatorCall{
                .type = call.type,
                .operand = std::make_unique<Expression>(substitute_expression(*call.operand, identifier, substitution)),
            };
        }
        else if (std::holds_alternative<BinaryOperatorCall>(expression))
        {
            auto &call = std::get<BinaryOperatorCall>(expression);
            return BinaryOperatorCall{
                .type = call.type,
                .lhs = std::make_unique<Expression>(substitute_expression(*call.lhs, identifier, substitution)),
                .rhs = std::make_unique<Expression>(substitute_expression(*call.rhs, identifier, substitution)),
            };
        }
        return expression;
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
            substituted.spec.postcondition.proposition = substitute_expression(substituted.spec.postcondition.proposition, identifier, expression);
            for (auto &pointer_declaration : substituted.spec.postcondition.heap.pointer_declarations)
            {
                pointer_declaration.base = std::make_shared<Expression>(substitute_expression(*pointer_declaration.base, identifier, expression));
                pointer_declaration.value = std::make_shared<Expression>(substitute_expression(*pointer_declaration.value, identifier, expression));
            }
        }
        return substituted;
    }
}
