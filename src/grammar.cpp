#include <grammar.hpp>

#include <string>
#include <iostream>
#include <algorithm>
#include <lexy/dsl.hpp>
#include <lexy/dsl/separator.hpp>
#include <lexy/dsl/brackets.hpp>
#include <lexy/callback.hpp>
#include <lexy/input/range_input.hpp>
#include <lexy/action/parse.hpp>
#include <lexy_ext/report_error.hpp>
#include <overload.hpp>
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

    struct subexpression : lexy::transparent_production
    {
        // Change the whitespace to include newlines, since the parens make them unambiguous.
        static constexpr auto whitespace = dsl::ascii::space | dsl::ascii::newline;
        static constexpr auto rule = dsl::recurse<struct expression>;
        static constexpr auto value = lexy::forward<Expression>;
    };

    struct expression : lexy::expression_production
    {
        static constexpr auto atom = dsl::p<literal> | dsl::p<identifier> | dsl::parenthesized(dsl::p<subexpression>);
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

        static constexpr auto impl = dsl::op(LEXY_LIT("=>"));

        struct implication : dsl::infix_op_right
        {
            static constexpr auto op = impl;
            using operand = equality;
        };

        using operation = implication;
        static constexpr auto value = lexy::callback<Expression>(
            [](int literal)
            { return literal; },
            [](std::string identifier)
            { return identifier; },
            [](Expression nested)
            { return nested; },
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
            { return BinaryOperatorCall{BinaryOperator::Neq, std::make_unique<Expression>(std::move(lhs)), std::make_unique<Expression>(std::move(rhs))}; },
            [](Expression lhs, lexy::op<impl>, Expression rhs)
            { return BinaryOperatorCall{BinaryOperator::Implies, std::make_unique<Expression>(std::move(lhs)), std::make_unique<Expression>(std::move(rhs))}; });
    };

    Expression parse_expr(std::string const &input)
    {
        auto result = lexy::parse<expression>(lexy::range_input(input.begin(), input.end()), lexy_ext::report_error);
        return std::move(result).value();
    }

    struct array_declaration
    {
        static constexpr auto rule = dsl::lit_c<'['> >> (dsl::p<identifier> + dsl::p<literal> + dsl::lit_c<']'>);
        static constexpr auto value = lexy::callback<ArrayDeclaration>(
            [](std::string identifier, int size)
            { return ArrayDeclaration{std::move(identifier), size}; });
    };

    struct pointer_declaration
    {
        static constexpr auto rule = dsl::lit_c<'<'> >> (dsl::p<expression> + dsl::lit_c<','> + dsl::p<literal> + dsl::lit_c<'>'> + LEXY_LIT("->") + dsl::p<expression>);
        static constexpr auto value = lexy::callback<PointerDeclaration>(
            [](Expression base, int offset, Expression expression)
            { return PointerDeclaration{
                  std::make_unique<Expression>(std::move(base)),
                  offset,
                  std::make_unique<Expression>(std::move(expression))}; });
    };

    struct predicate_call
    {
        // Normally we'd use a list of expressions, but the paper specifies that the arguments can only be identifiers.
        static constexpr auto args = dsl::list(dsl::p<identifier>, dsl::sep(dsl::comma));
        static constexpr auto rule = dsl::p<identifier> + dsl::lit_c<'('> + args + dsl::lit_c<')'>;
        static constexpr auto value = lexy::callback<PredicateCall>(
            [](std::string identifier, std::vector<Identifier> parameters)
            { return PredicateCall{std::move(identifier), std::move(parameters)}; });
    };

    using HeapElement = std::variant<ArrayDeclaration, PointerDeclaration, PredicateCall>;

    struct heap_element
    {
        // array_declaration and pointer_declaration are identified by [ adn <, respectively, so we use else_ to match the predicate call.
        static constexpr auto rule = dsl::p<array_declaration> | dsl::p<pointer_declaration> | dsl::else_ >> dsl::p<predicate_call>;
        static constexpr auto value = lexy::forward<HeapElement>;
    };

    struct heap_elem_test
    {
        // array_declaration and pointer_declaration are identified by [ adn <, respectively, so we use else_ to match the predicate call.
        static constexpr auto rule = dsl::p<pointer_declaration>;
        static constexpr auto value = lexy::forward<HeapElement>;
    };

    struct heap
    {
        static constexpr auto rule = dsl::terminator(dsl::lit_c<';'>).list(dsl::p<heap_elem_test>, dsl::sep(dsl::comma));
        static constexpr auto value = lexy::fold_inplace<Heap>(
            []
            { return Heap{}; },
            [](Heap &heap, auto elem)
            {
                std::visit(
                    overloaded{
                        [&heap](ArrayDeclaration elem)
                        { heap.array_declarations.push_back(std::move(elem)); },
                        [&heap](PointerDeclaration elem)
                        { heap.pointer_declarations.push_back(std::move(elem)); },
                        [&heap](PredicateCall elem)
                        { heap.predicate_calls.push_back(std::move(elem)); }},
                    std::move(elem));
            });
    };

    Heap parse_heap(std::string const &input)
    {
        auto const_result = lexy::parse<heap>(lexy::range_input(input.begin(), input.end()), lexy_ext::report_error);
        return std::move(const_result).value();
    }

    struct assertion
    {
        static constexpr auto rule = dsl::curly_bracketed(dsl::p<expression> + dsl::lit_c<';'> + dsl::p<heap>);
        static constexpr auto value = lexy::construct<Assertion>;
    };

    struct int_type
    {
        static constexpr auto rule = LEXY_LIT("int");
        static constexpr auto value = lexy::callback<Type>(
            []()
            { return Type::Int; });
    };

    struct loc_type
    {
        static constexpr auto rule = LEXY_LIT("loc");
        static constexpr auto value = lexy::callback<Type>(
            []()
            { return Type::Loc; });
    };

    struct type
    {
        static constexpr auto rule = dsl::p<int_type> | dsl::p<loc_type>;
        static constexpr auto value = lexy::forward<Type>;
    };

    struct typed_variable
    {
        static constexpr auto rule = dsl::p<type> + dsl::p<identifier>;
        static constexpr auto value = lexy::construct<TypedVariable>;
    };

    struct function_parameters
    {
        static constexpr auto rule = dsl::parenthesized.opt_list(dsl::p<typed_variable>, dsl::sep(dsl::comma));
        static constexpr auto value = lexy::as_list<std::vector<TypedVariable>>;
    };

    struct function_signature
    {
        static constexpr auto rule = dsl::p<identifier> + dsl::p<function_parameters>;
        static constexpr auto value = lexy::construct<FunctionSignature>;
    };

    struct function_specification
    {
        static constexpr auto whitespace = dsl::ascii::space | dsl::ascii::newline;
        static constexpr auto rule = dsl::p<assertion> + dsl::p<function_signature> + dsl::p<assertion>;
        static constexpr auto value = lexy::construct<FunctionSpecification>;
    };

    struct query
    {
        static constexpr auto whitespace = dsl::ascii::space | dsl::ascii::newline;
        static constexpr auto rule = dsl::p<function_specification>;
        static constexpr auto value = lexy::forward<FunctionSpecification>;
    };

    Query parse_query(std::string const &input)
    {
        auto const_result = lexy::parse<query>(lexy::range_input(input.begin(), input.end()), lexy_ext::report_error);
        if (const_result)
            std::cout << "Parsed int_type\n"
                      << std::endl;
        return Query{};
    }
}
