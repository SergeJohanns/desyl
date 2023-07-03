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
}
