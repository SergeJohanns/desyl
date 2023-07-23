#include <read.hpp>

#include <overload.hpp>
#include <iostream>
#include <sstream>

namespace desyl
{
    ReadContinuation::ReadContinuation(Identifier const &output, PointerDeclaration const &pointer)
    {
        std::ostringstream stream;
        stream << output << " = *(" << stringify_expression(*pointer.base) << " + " << pointer.offset << ");" << std::endl;
        line = stream.str();
    }

    Program ReadContinuation::join(std::vector<Program> const &result) const
    {
        return result[0].add_lines(line);
    }

    Identifier substitute(Identifier &target, Identifier const &before, Identifier const &after)
    {
        return target == before ? after : target;
    }

    Expression substitute(Expression &target, Identifier const &before, Identifier const &after)
    {
        return std::visit(
            overloaded{
                [&](Identifier &expr)
                { return Expression{substitute(expr, before, after)}; },
                [&](Literal &expr)
                { return Expression(expr); },
                [&](SetLiteral &expr)
                {
                    std::vector<Expression> new_elements;
                    for (auto &element : expr.elements)
                    {
                        new_elements.push_back(substitute(element, before, after));
                    }
                    return Expression{SetLiteral{new_elements}};
                },
                [&](UnaryOperatorCall &expr)
                {
                    return Expression{UnaryOperatorCall{
                        expr.type,
                        std::make_shared<Expression>(substitute(*expr.operand, before, after)),
                    }};
                },
                [&](BinaryOperatorCall &identifier)
                {
                    return Expression{BinaryOperatorCall{
                        identifier.type,
                        std::make_shared<Expression>(substitute(*identifier.lhs, before, after)),
                        std::make_shared<Expression>(substitute(*identifier.rhs, before, after)),
                    }};
                },
            },
            target);
    }

    void substitute(Assertion &target, Identifier const &before, Identifier const &after)
    {
        for (auto &expression : target.proposition)
        {
            expression = substitute(expression, before, after);
        }
        for (auto &pointer : target.heap.pointer_declarations)
        {
            pointer.base = std::make_shared<Expression>(substitute(*pointer.base, before, after));
            pointer.value = std::make_shared<Expression>(substitute(*pointer.value, before, after));
        }
        for (auto &predicate : target.heap.predicate_calls)
        {
            std::vector<Identifier> new_args;
            for (auto &arg : predicate.args)
            {
                arg = substitute(arg, before, after);
            }
        }
    }

    std::vector<Derivation> ReadRule::apply(Goal const &goal) const
    {
        std::vector<Derivation> result;
        auto variables = goal.variables();
        auto const all = variables.all();
        auto const ghosts = variables.ghosts();
        for (auto const &pointer : goal.spec.precondition.heap.pointer_declarations)
        {
            if ((*pointer.value).index() != EXPRESSION_IDENTIFIER_INDEX)
            {
                continue;
            }
            auto const &identifier = std::get<EXPRESSION_IDENTIFIER_INDEX>(*pointer.value);
            if (ghosts.find(identifier) == ghosts.end())
            {
                continue;
            }
            Identifier new_var;
            for (int i = 0;; ++i)
            {
                new_var = identifier + "_" + std::to_string(i);
                if (all.find(new_var) == all.end())
                {
                    break;
                }
            }
            auto new_goal(goal);
            new_goal.environment.insert(new_var);
            substitute(new_goal.spec.precondition, identifier, new_var);
            substitute(new_goal.spec.postcondition, identifier, new_var);
            Derivation deriv{
                .goals = std::vector<Goal>{std::move(new_goal)},
                .continuation = std::make_unique<ReadContinuation>(ReadContinuation(new_var, pointer)),
            };
            result.push_back(std::move(deriv));
        }
        return result;
    }
}
