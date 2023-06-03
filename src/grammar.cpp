#include <grammar.hpp>

#include <string>
#include <iostream>
#include <lexy/dsl.hpp>
#include <lexy/callback.hpp>
#include <lexy/input/range_input.hpp>
#include <lexy/action/parse.hpp>
#include <lexy_ext/report_error.hpp>
#include <desyl/ast.hpp>

namespace desyl
{
    namespace
    {
        namespace dsl = lexy::dsl;
    }

    struct literal
    {
        static constexpr auto rule = dsl::integer<int>;
        static constexpr auto value = lexy::as_integer<int>;
    };

    struct identifier
    {
        static constexpr auto rule = dsl::identifier(dsl::ascii::alpha);
        static constexpr auto value = lexy::as_string<std::string>;
    };

    struct expression : lexy::expression_production
    {
        static constexpr auto atom = dsl::p<literal> | dsl::p<identifier>;
        static constexpr auto whitespace = dsl::ascii::space;

        static constexpr auto min = dsl::op(dsl::lit_c<'-'>);
        static constexpr auto neg = dsl::op(dsl::lit_c<'!'>);

        struct prefix : dsl::prefix_op
        {
            static constexpr auto op = min / neg;
            using operand = dsl::atom;
        };

        static constexpr auto lt = dsl::op(dsl::lit_c<'<'>);
        static constexpr auto leq = dsl::op(LEXY_LIT("<="));
        static constexpr auto gt = dsl::op(dsl::lit_c<'>'>);
        static constexpr auto geq = dsl::op(LEXY_LIT(">="));

        struct ordering : dsl::infix_op_left
        {
            static constexpr auto op = leq / lt / geq / gt;
            using operand = prefix;
        };

        static constexpr auto eq = dsl::op(LEXY_LIT("=="));
        static constexpr auto neq = dsl::op(LEXY_LIT("!="));

        struct equality : dsl::infix_op_left
        {
            static constexpr auto op = eq / neq;
            using operand = ordering;
        };

        using operation = equality;
        static constexpr auto value = lexy::callback<Expression>(
            [](int literal)
            { return literal; },
            [](std::string identifier)
            { return identifier; },
            [](lexy::op<min>, Expression operand)
            { return UnaryOperatorCall{UnaryOperator::Neg, std::make_unique<Expression>(std::move(operand))}; },
            [](lexy::op<neg>, Expression operand)
            { return UnaryOperatorCall{UnaryOperator::Not, std::make_unique<Expression>(std::move(operand))}; },
            [](Expression lhs, lexy::op<lt>, Expression rhs)
            { return BinaryOperatorCall{BinaryOperator::Lt, std::make_unique<Expression>(std::move(lhs)), std::make_unique<Expression>(std::move(rhs))}; },
            [](Expression lhs, lexy::op<leq>, Expression rhs)
            { return BinaryOperatorCall{BinaryOperator::Leq, std::make_unique<Expression>(std::move(lhs)), std::make_unique<Expression>(std::move(rhs))}; },
            [](Expression lhs, lexy::op<gt>, Expression rhs)
            { return BinaryOperatorCall{BinaryOperator::Gt, std::make_unique<Expression>(std::move(lhs)), std::make_unique<Expression>(std::move(rhs))}; },
            [](Expression lhs, lexy::op<geq>, Expression rhs)
            { return BinaryOperatorCall{BinaryOperator::Geq, std::make_unique<Expression>(std::move(lhs)), std::make_unique<Expression>(std::move(rhs))}; },
            [](Expression lhs, lexy::op<eq>, Expression rhs)
            { return BinaryOperatorCall{BinaryOperator::Eq, std::make_unique<Expression>(std::move(lhs)), std::make_unique<Expression>(std::move(rhs))}; },
            [](Expression lhs, lexy::op<neq>, Expression rhs)
            { return BinaryOperatorCall{BinaryOperator::Neq, std::make_unique<Expression>(std::move(lhs)), std::make_unique<Expression>(std::move(rhs))}; });
    };

    Expression parse_expr(std::string const &input)
    {
        auto result = lexy::parse<expression>(lexy::range_input(input.begin(), input.end()), lexy_ext::report_error);
        return std::move(result).value();
    }
}
