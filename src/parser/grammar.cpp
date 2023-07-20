#include <grammar.hpp>

#include <string>
#include <iostream>
#include <algorithm>
#include <stack>
#include <lexy/dsl.hpp>
#include <lexy/dsl/separator.hpp>
#include <lexy/dsl/brackets.hpp>
#include <lexy/callback.hpp>
#include <lexy/input/file.hpp>
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

        static constexpr auto mul = dsl::op(dsl::lit_c<'*'>);
        static constexpr auto div = dsl::op(dsl::lit_c<'/'>);
        static constexpr auto mod = dsl::op(dsl::lit_c<'%'>);

        struct multiplication : dsl::infix_op_left
        {
            static constexpr auto op = mul / div / mod;
            using operand = prefix;
        };

        static constexpr auto add = dsl::op(dsl::lit_c<'+'>);
        static constexpr auto sub = dsl::op(dsl::lit_c<'-'>);

        struct addition : dsl::infix_op_left
        {
            static constexpr auto op = add / sub;
            using operand = multiplication;
        };

        static constexpr auto lt = dsl::op(dsl::lit_c<'<'>);
        static constexpr auto leq = dsl::op(LEXY_LIT("<="));
        static constexpr auto gt = dsl::op(dsl::lit_c<'>'>);
        static constexpr auto geq = dsl::op(LEXY_LIT(">="));

        struct ordering : dsl::infix_op_left
        {
            static constexpr auto op = leq / lt / geq / gt;
            using operand = addition;
        };

        static constexpr auto eq = dsl::op(LEXY_LIT("=="));
        static constexpr auto neq = dsl::op(LEXY_LIT("!="));

        struct equality : dsl::infix_op_left
        {
            static constexpr auto op = eq / neq;
            using operand = ordering;
        };

        static constexpr auto land = dsl::op(LEXY_LIT("&&"));

        struct logical_and : dsl::infix_op_left
        {
            static constexpr auto op = land;
            using operand = equality;
        };

        static constexpr auto lor = dsl::op(LEXY_LIT("||"));

        struct logical_or : dsl::infix_op_left
        {
            static constexpr auto op = lor;
            using operand = logical_and;
        };

        static constexpr auto impl = dsl::op(LEXY_LIT("=>"));

        struct implication : dsl::infix_op_right
        {
            static constexpr auto op = impl;
            using operand = logical_or;
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
            [](Expression lhs, lexy::op<mul>, Expression rhs)
            { return BinaryOperatorCall{BinaryOperator::Mul, std::make_unique<Expression>(std::move(lhs)), std::make_unique<Expression>(std::move(rhs))}; },
            [](Expression lhs, lexy::op<div>, Expression rhs)
            { return BinaryOperatorCall{BinaryOperator::Div, std::make_unique<Expression>(std::move(lhs)), std::make_unique<Expression>(std::move(rhs))}; },
            [](Expression lhs, lexy::op<mod>, Expression rhs)
            { return BinaryOperatorCall{BinaryOperator::Mod, std::make_unique<Expression>(std::move(lhs)), std::make_unique<Expression>(std::move(rhs))}; },
            [](Expression lhs, lexy::op<add>, Expression rhs)
            { return BinaryOperatorCall{BinaryOperator::Add, std::make_unique<Expression>(std::move(lhs)), std::make_unique<Expression>(std::move(rhs))}; },
            [](Expression lhs, lexy::op<sub>, Expression rhs)
            { return BinaryOperatorCall{BinaryOperator::Sub, std::make_unique<Expression>(std::move(lhs)), std::make_unique<Expression>(std::move(rhs))}; },
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
            [](Expression lhs, lexy::op<land>, Expression rhs)
            { return BinaryOperatorCall{BinaryOperator::And, std::make_unique<Expression>(std::move(lhs)), std::make_unique<Expression>(std::move(rhs))}; },
            [](Expression lhs, lexy::op<lor>, Expression rhs)
            { return BinaryOperatorCall{BinaryOperator::Or, std::make_unique<Expression>(std::move(lhs)), std::make_unique<Expression>(std::move(rhs))}; },
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
        static constexpr auto rule = dsl::lit_c<'['> >> (dsl::p<identifier> + dsl::lit_c<','> + dsl::p<literal> + dsl::lit_c<']'>);
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

    struct predicate_argument_list
    {
        // Normally we'd use a list of expressions, but the paper specifies that the arguments can only be identifiers.
        static constexpr auto rule = dsl::list(dsl::p<identifier>, dsl::sep(dsl::comma));
        static constexpr auto value = lexy::as_list<std::vector<Identifier>>;
    };

    struct predicate_call
    {
        static constexpr auto rule = dsl::p<identifier> + dsl::lit_c<'('> + dsl::p<predicate_argument_list> + dsl::lit_c<')'>;
        static constexpr auto value = lexy::callback<PredicateCall>(
            [](std::string identifier, std::vector<Identifier> parameters)
            { return PredicateCall{std::move(identifier), std::move(parameters)}; });
    };

    struct emp
    {
        static constexpr auto rule = LEXY_LIT("emp");
        static constexpr auto value = lexy::callback<std::monostate>(
            []
            { return std::monostate{}; });
    };

    using HeapElement = std::variant<ArrayDeclaration, PointerDeclaration, PredicateCall, std::monostate>;

    struct heap_element
    {
        // array_declaration and pointer_declaration are identified by [ adn <, respectively, so we use else_ to match the predicate call.
        static constexpr auto rule = dsl::p<array_declaration> | dsl::p<pointer_declaration> | dsl::p<emp> | dsl::else_ >> dsl::p<predicate_call>;
        static constexpr auto value = lexy::callback<HeapElement>(
            [](ArrayDeclaration elem)
            { return elem; },
            [](PointerDeclaration elem)
            { return elem; },
            [](PredicateCall elem)
            { return elem; },
            [](std::monostate elem)
            { return elem; });
    };

    struct heap
    {
        static constexpr auto rule = dsl::terminator(dsl::lit_c<';'>).list(dsl::p<heap_element>, dsl::sep(dsl::vbar));
        static constexpr auto value =
            lexy::fold_inplace<Heap>(
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
                            { heap.predicate_calls.push_back(std::move(elem)); },
                            [&heap](std::monostate) {}},
                        std::move(elem));
                });
    };

    Heap parse_heap(std::string const &input)
    {
        auto const_result = lexy::parse<heap>(lexy::range_input(input.begin(), input.end()), lexy_ext::report_error);
        return std::move(const_result).value();
    }

    struct proposition
    {
        static constexpr auto rule = dsl::p<expression>;
        static constexpr auto value = lexy::callback<Proposition>(
            [](Expression expression)
            {
                std::stack<Expression> stack;
                stack.push(std::move(expression));
                std::vector<Expression> result;
                while (!stack.empty())
                {
                    auto top = std::move(stack.top());
                    stack.pop();
                    auto binary_operator_call = std::get_if<BinaryOperatorCall>(&top);
                    auto var = std::get_if<Identifier>(&top);
                    if (binary_operator_call && binary_operator_call->type == BinaryOperator::And)
                    {
                        stack.push(std::move(*binary_operator_call->rhs));
                        stack.push(std::move(*binary_operator_call->lhs));
                    }
                    else if (!var || *var != "true")
                    {
                        result.push_back(std::move(top));
                    }
                }
                return Proposition{std::move(result)};
            });
    };

    struct assertion
    {
        static constexpr auto rule = dsl::curly_bracketed(dsl::p<proposition> + dsl::lit_c<';'> + dsl::p<heap>);
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

    struct functions
    {
        static constexpr auto whitespace = dsl::ascii::space | dsl::ascii::newline;
        static constexpr auto rule = dsl::p<function_specification>;
        static constexpr auto value = lexy::as_list<std::vector<FunctionSpecification>>;
        // static constexpr auto value = lexy::callback<std::vector<FunctionSpecification>>(
        //     [](FunctionSpecification function_specification)
        //     { return std::vector<FunctionSpecification>{std::move(function_specification)}; });
    };

    struct algebraic_int_type
    {
        static constexpr auto rule = LEXY_LIT("int");
        static constexpr auto value = lexy::callback<AlgebraicType>(
            []()
            { return AlgebraicType::Int; });
    };

    struct algebraic_loc_type
    {
        static constexpr auto rule = LEXY_LIT("loc");
        static constexpr auto value = lexy::callback<AlgebraicType>(
            []()
            { return AlgebraicType::Loc; });
    };

    struct algebraic_set_type
    {
        static constexpr auto rule = LEXY_LIT("set");
        static constexpr auto value = lexy::callback<AlgebraicType>(
            []()
            { return AlgebraicType::Set; });
    };

    struct algebraic_type
    {
        static constexpr auto rule = dsl::p<algebraic_int_type> | dsl::p<algebraic_loc_type> | dsl::p<algebraic_set_type>;
        static constexpr auto value = lexy::forward<AlgebraicType>;
    };

    struct algebraic_typed_variable
    {
        static constexpr auto rule = dsl::p<algebraic_type> + dsl::p<identifier>;
        static constexpr auto value = lexy::construct<AlgebraicTypedVariable>;
    };

    struct predicate_parameters
    {
        static constexpr auto rule = dsl::parenthesized.opt_list(dsl::p<algebraic_typed_variable>, dsl::sep(dsl::comma));
        static constexpr auto value = lexy::as_list<std::vector<AlgebraicTypedVariable>>;
    };

    struct clause
    {
        static constexpr auto rule = dsl::vbar + dsl::p<expression> + LEXY_LIT("|=>") + dsl::p<assertion>;
        static constexpr auto value = lexy::construct<Clause>;
    };

    struct clauses
    {
        static constexpr auto whitespace = dsl::ascii::space | dsl::ascii::newline;
        static constexpr auto rule = dsl::curly_bracketed.list(dsl::p<clause>);
        static constexpr auto value = lexy::as_list<std::vector<Clause>>;
    };

    struct predicate_specification
    {
        static constexpr auto whitespace = dsl::ascii::space | dsl::ascii::newline;
        static constexpr auto rule = LEXY_LIT("predicate") + dsl::p<identifier> + dsl::p<predicate_parameters> + dsl::p<clauses>;
        static constexpr auto value = lexy::construct<Predicate>;
    };

    struct predicates
    {
        static constexpr auto whitespace = dsl::ascii::space | dsl::ascii::newline;
        static constexpr auto rule = dsl::list(dsl::p<predicate_specification>, dsl::sep(dsl::newline));
        static constexpr auto value = lexy::as_list<std::vector<Predicate>>;
    };

    struct query
    {
        static constexpr auto whitespace = dsl::ascii::space | dsl::ascii::newline;
        static constexpr auto rule = dsl::p<predicates> + dsl::p<functions>;
        static constexpr auto value = lexy::construct<Query>;
    };

    Query parse_query(std::string const &input)
    {
        auto const &file = lexy::read_file(input.c_str());
        if (!file)
        {
            std::cerr << "Error: could not read file " << input << std::endl;
            std::exit(1);
        }
        auto const_result = lexy::parse<query>(file.buffer(), lexy_ext::report_error);
        return std::move(const_result).value();
    }
}
