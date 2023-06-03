#include <desyl/desyllib.hpp>

#include <desyl/ast.hpp>
#include <grammar.hpp>

namespace desyl
{
    Query parse(std::string const &)
    {
        return Query{};
    }

    std::string stringify_op(UnaryOperator const &op)
    {
        return op == UnaryOperator::Neg ? std::string{"-"} : std::string{"!"};
    }

    std::string stringify_op(BinaryOperator const &op)
    {
        switch (op)
        {
        case BinaryOperator::Lt:
            return std::string{"<"};
        case BinaryOperator::Leq:
            return std::string{"<="};
        case BinaryOperator::Gt:
            return std::string{">"};
        case BinaryOperator::Geq:
            return std::string{">="};
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

    std::string
    example(std::string const &input)
    {
        return std::visit(stringify, parse_expr(input));
    }
}
