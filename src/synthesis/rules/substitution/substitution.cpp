#include <substitution.hpp>

namespace desyl
{
    Expression substitute_expression(Expression const &expression, Identifier const &identifier, Expression const &substitution)
    {
        if (std::holds_alternative<Identifier>(expression) && std::get<Identifier>(expression) == identifier)
        {
            return substitution;
        }
        else if (std::holds_alternative<SetLiteral>(expression))
        {
            auto &set = std::get<SetLiteral>(expression);
            auto substituted = SetLiteral{};
            for (auto const &element : set.elements)
            {
                substituted.elements.push_back(substitute_expression(element, identifier, substitution));
            }
            return substituted;
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

    PointerDeclaration substitute(PointerDeclaration const &pointer, Substitutions const &substitutions)
    {
        PointerDeclaration substituted(pointer);
        substituted.base = std::make_shared<Expression>(substitute(*substituted.base, substitutions));
        substituted.value = std::make_shared<Expression>(substitute(*substituted.value, substitutions));
        return substituted;
    }

    ArrayDeclaration substitute(ArrayDeclaration const &array, Substitutions const &substitutions)
    {
        ArrayDeclaration substituted(array);
        substituted.name = std::get<Identifier>(substitute(substituted.name, substitutions));
        return substituted;
    }

    PredicateCall substitute(PredicateCall const &predicate_call, Substitutions const &substitutions)
    {
        PredicateCall substituted(predicate_call);
        for (size_t i = 0; i < substituted.args.size(); ++i)
        {
            substituted.args[i] = std::get<Identifier>(substitute(substituted.args[i], substitutions));
        }
        return substituted;
    }

    Heap substitute(Heap const &heap, Substitutions const &substitutions)
    {
        Heap substituted;
        for (auto &pointer_declaration : heap.pointer_declarations)
        {
            substituted.pointer_declarations.push_back(substitute(pointer_declaration, substitutions));
        }
        for (auto &array_declaration : heap.array_declarations)
        {
            substituted.array_declarations.push_back(substitute(array_declaration, substitutions));
        }
        for (auto &predicate_call : heap.predicate_calls)
        {
            substituted.predicate_calls.push_back(substitute(predicate_call, substitutions));
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
