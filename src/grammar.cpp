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

        using operation = ordering;
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
            { return BinaryOperatorCall{BinaryOperator::Geq, std::make_unique<Expression>(std::move(lhs)), std::make_unique<Expression>(std::move(rhs))}; });

        // static constexpr auto add = dsl::op(dsl::lit_c<'+'>);
        // static constexpr auto sub = dsl::op(dsl::lit_c<'-'>);

        // struct sum : dsl::infix_op_left
        // {
        //     static constexpr auto op = add / sub;
        //     using operand = product;
        //     static constexpr auto value = lexy::as_aggregate<BinaryOperatorCall>;
        // };
    };

    Expression parse_expr(std::string const &input)
    {
        auto result = lexy::parse<expression>(lexy::range_input(input.begin(), input.end()), lexy_ext::report_error);
        return std::move(result).value();
    }
}
