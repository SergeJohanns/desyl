#include <desyl/ast.hpp>

namespace desyl
{
    std::string stringify_op(UnaryOperator const &op)
    {
        return op == UnaryOperator::Neg ? std::string{"-"} : std::string{"!"};
    }

    std::string stringify_op(BinaryOperator const &op)
    {
        switch (op)
        {
        case BinaryOperator::Mul:
            return std::string{"*"};
        case BinaryOperator::Div:
            return std::string{"/"};
        case BinaryOperator::Mod:
            return std::string{"%"};
        case BinaryOperator::Add:
            return std::string{"+"};
        case BinaryOperator::Sub:
            return std::string{"-"};
        case BinaryOperator::Lt:
            return std::string{"<"};
        case BinaryOperator::Leq:
            return std::string{"<="};
        case BinaryOperator::Gt:
            return std::string{">"};
        case BinaryOperator::Geq:
            return std::string{">="};
        case BinaryOperator::Eq:
            return std::string{"=="};
        case BinaryOperator::Neq:
            return std::string{"!="};
        case BinaryOperator::And:
            return std::string{"&&"};
        case BinaryOperator::Or:
            return std::string{"||"};
        case BinaryOperator::Implies:
            return std::string{"=>"};
        default:
            return std::string{};
        }
    }

    // Standard overload resolution (https://www.modernescpp.com/index.php/visiting-a-std-variant-with-the-overload-pattern)
    // doesn't like recursive lambdas, so we use a plain struct
    struct StringifyVisitor
    {
        const auto operator()(Literal const &literal) const
        {
            return std::to_string(literal);
        }

        const auto operator()(Identifier const &identifier) const
        {
            return identifier;
        }

        const auto operator()(UnaryOperatorCall const &call) const
        {
            return std::string{"("} + stringify_op(call.type) + std::visit(*this, *call.operand) + std::string{")"};
        }

        const auto operator()(BinaryOperatorCall const &call) const
        {
            return std::string{"("} + std::visit(*this, *call.lhs) + std::string{" "} + stringify_op(call.type) + std::string{" "} + std::visit(*this, *call.rhs) + std::string{")"};
        }
    };

    constexpr StringifyVisitor stringify;

    std::string stringify_expression(Expression const &expression)
    {
        return std::visit(stringify, expression);
    }

    bool UnaryOperatorCall::operator==(UnaryOperatorCall const &other) const
    {
        return type == other.type && *operand == *other.operand;
    }

    bool BinaryOperatorCall::operator==(BinaryOperatorCall const &other) const
    {
        return type == other.type && *lhs == *other.lhs && *rhs == *other.rhs;
    }

    bool PointerDeclaration::operator==(PointerDeclaration const &other) const
    {
        return *base == *other.base && offset == other.offset && *value == *other.value;
    }
}
