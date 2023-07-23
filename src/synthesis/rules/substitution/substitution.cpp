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

    Expression substitute(Expression const &expression, Substitutions const &substitutions)
    {
        Expression substituted(expression);
        for (auto const &[identifier, expression] : substitutions)
        {
            substituted = substitute_expression(substituted, identifier, expression);
        }
        return substituted;
    }

    Proposition substitute(Proposition const &proposition, Substitutions const &substitutions)
    {
        Proposition substituted(proposition);
        for (size_t i = 0; i < substituted.size(); ++i)
        {
            substituted[i] = substitute(substituted[i], substitutions);
        }
        return substituted;
    }

    Heap substitute(Heap const &heap, Substitutions const &substitutions)
    {
        Heap substituted(heap);
        for (auto &pointer_declaration : substituted.pointer_declarations)
        {
            pointer_declaration.base = std::make_shared<Expression>(substitute(*pointer_declaration.base, substitutions));
            pointer_declaration.value = std::make_shared<Expression>(substitute(*pointer_declaration.value, substitutions));
        }
        for (auto &array_declaration : substituted.array_declarations)
        {
            array_declaration.name = std::get<Identifier>(substitute(array_declaration.name, substitutions));
        }
        for (auto &predicate_call : substituted.predicate_calls)
        {
            for (auto &argument : predicate_call.args)
            {
                argument = std::get<Identifier>(substitute(argument, substitutions));
            }
        }
        return substituted;
    }

    Assertion substitute(Assertion const &assertion, Substitutions const &substitutions)
    {
        Assertion substituted(assertion);
        substituted.proposition = substitute(substituted.proposition, substitutions);
        substituted.heap = substitute(substituted.heap, substitutions);
        return substituted;
    }

    Goal substitute(Goal const &goal, Substitutions const &substitutions)
    {
        Goal substituted(goal);
        substituted.spec.postcondition = substitute(goal.spec.postcondition, substitutions);
        return substituted;
    }
}
