#include <grammar.hpp>

#include <string>
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

    struct production : lexy::expression_production
    {
        static constexpr auto atom = dsl::p<literal> | dsl::p<identifier>;

        static constexpr auto min = dsl::op(dsl::lit_c<'-'>);
        static constexpr auto neg = dsl::op(dsl::lit_c<'!'>);

        struct prefix : dsl::prefix_op
        {
            static constexpr auto op = min / neg;
            using operand = dsl::atom;
        };

        using operation = prefix;
        static constexpr auto value = lexy::callback<Expression>(
            [](int literal)
            { return literal; },
            [](std::string identifier)
            { return identifier; },
            [](lexy::op<min>, Expression operand)
            { return UnaryOperatorCall{UnaryOperator::Neg, std::make_unique<Expression>(std::move(operand))}; },
            [](lexy::op<neg>, Expression operand)
            { return UnaryOperatorCall{UnaryOperator::Not, std::make_unique<Expression>(std::move(operand))}; });

        // static constexpr auto mult = dsl::op(dsl::lit_c<'*'>);
        // static constexpr auto div = dsl::op(dsl::lit_c<'/'>);
        // static constexpr auto rem = dsl::op(dsl::lit_c<'%'>);

        // struct product : dsl::infix_op_left
        // {
        //     static constexpr auto op = mult / div / rem;
        //     using operand = prefix;
        //     static constexpr auto value = lexy::as_aggregate<BinaryOperatorCall>;
        // };

        // static constexpr auto add = dsl::op(dsl::lit_c<'+'>);
        // static constexpr auto sub = dsl::op(dsl::lit_c<'-'>);

        // struct sum : dsl::infix_op_left
        // {
        //     static constexpr auto op = add / sub;
        //     using operand = product;
        //     static constexpr auto value = lexy::as_aggregate<BinaryOperatorCall>;
        // };
    };

    std::variant<Literal, Identifier> example1(std::string const &input)
    {
        auto result = lexy::parse<literal>(lexy::range_input(input.begin(), input.end()), lexy_ext::report_error);
        return result.value();
    }

    Expression example2(std::string const &input)
    {
        auto result = lexy::parse<production>(lexy::range_input(input.begin(), input.end()), lexy_ext::report_error);
        return std::move(result).value();
    }
}
