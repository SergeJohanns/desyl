#include <expand_implied.hpp>

#include <generic_continuations/identity_continuation.hpp>
#include <overload.hpp>
#include <algorithm>
#include <iostream>

namespace desyl
{
    std::vector<Expression> implied(Expression const &expression);

    std::vector<Expression> implied_inner(Literal const &)
    {
        return {};
    }

    std::vector<Expression> implied_inner(Identifier const &)
    {
        return {};
    }

    std::vector<Expression> implied_inner(UnaryOperatorCall const &unary_operation)
    {
        std::vector<Expression> result;
        for (auto &implied : implied(*unary_operation.operand))
        {
            result.push_back(UnaryOperatorCall{
                .type = unary_operation.type,
                .operand = std::make_shared<Expression>(implied),
            });
        }
        if (unary_operation.type == UnaryOperator::Neg || !std::holds_alternative<BinaryOperatorCall>(*unary_operation.operand))
        {
            return result;
        }
        auto const &binary_operation(std::get<BinaryOperatorCall>(*unary_operation.operand));
        switch (binary_operation.type)
        {
        case BinaryOperator::Lt:
            result.push_back(BinaryOperatorCall{
                .type = BinaryOperator::Geq,
                .lhs = binary_operation.lhs,
                .rhs = binary_operation.rhs,
            });
            break;
        case BinaryOperator::Leq:
            result.push_back(BinaryOperatorCall{
                .type = BinaryOperator::Gt,
                .lhs = binary_operation.lhs,
                .rhs = binary_operation.rhs,
            });
            break;
        case BinaryOperator::Gt:
            result.push_back(BinaryOperatorCall{
                .type = BinaryOperator::Leq,
                .lhs = binary_operation.lhs,
                .rhs = binary_operation.rhs,
            });
            break;
        case BinaryOperator::Geq:
            result.push_back(BinaryOperatorCall{
                .type = BinaryOperator::Lt,
                .lhs = binary_operation.lhs,
                .rhs = binary_operation.rhs,
            });
            break;
        case BinaryOperator::Eq:
            result.push_back(BinaryOperatorCall{
                .type = BinaryOperator::Neq,
                .lhs = binary_operation.lhs,
                .rhs = binary_operation.rhs,
            });
            break;
        case BinaryOperator::Neq:
            result.push_back(BinaryOperatorCall{
                .type = BinaryOperator::Eq,
                .lhs = binary_operation.lhs,
                .rhs = binary_operation.rhs,
            });
            break;
        case BinaryOperator::And:
            result.push_back(BinaryOperatorCall{
                .type = BinaryOperator::And,
                .lhs = std::make_shared<Expression>(UnaryOperatorCall{
                    .type = UnaryOperator::Neg,
                    .operand = binary_operation.lhs,
                }),
                .rhs = std::make_shared<Expression>(UnaryOperatorCall{
                    .type = UnaryOperator::Neg,
                    .operand = binary_operation.rhs,
                }),
            });
            break;
        case BinaryOperator::Or:
            result.push_back(
                UnaryOperatorCall{
                    .type = UnaryOperator::Neg,
                    .operand = binary_operation.lhs,
                });
            result.push_back(
                UnaryOperatorCall{
                    .type = UnaryOperator::Neg,
                    .operand = binary_operation.rhs,
                });
            break;
        case BinaryOperator::Implies:
            result.push_back(
                UnaryOperatorCall{
                    .type = UnaryOperator::Neg,
                    .operand = binary_operation.lhs,
                });
            result.push_back(*binary_operation.rhs);
            break;
        default:
            break;
        }
        return result;
    }

    std::vector<Expression> implied_inner(BinaryOperatorCall const &binary_operation)
    {
        std::vector<Expression> result;
        for (auto &implied : implied(*binary_operation.lhs))
        {
            result.push_back(BinaryOperatorCall{
                .type = binary_operation.type,
                .lhs = std::make_shared<Expression>(implied),
                .rhs = binary_operation.rhs,
            });
        }
        for (auto &implied : implied(*binary_operation.rhs))
        {
            result.push_back(BinaryOperatorCall{
                .type = binary_operation.type,
                .lhs = binary_operation.lhs,
                .rhs = std::make_shared<Expression>(implied),
            });
        }
        switch (binary_operation.type)
        {
        case BinaryOperator::Lt:
            return {BinaryOperatorCall{
                        .type = BinaryOperator::Leq,
                        .lhs = binary_operation.lhs,
                        .rhs = binary_operation.rhs,
                    },
                    BinaryOperatorCall{
                        .type = BinaryOperator::Gt,
                        .lhs = binary_operation.rhs,
                        .rhs = binary_operation.lhs,
                    }};
        case BinaryOperator::Leq:
            return {
                BinaryOperatorCall{
                    .type = BinaryOperator::Geq,
                    .lhs = binary_operation.rhs,
                    .rhs = binary_operation.lhs,
                }};
        case BinaryOperator::Gt:
            return {BinaryOperatorCall{
                        .type = BinaryOperator::Geq,
                        .lhs = binary_operation.lhs,
                        .rhs = binary_operation.rhs,
                    },
                    BinaryOperatorCall{
                        .type = BinaryOperator::Lt,
                        .lhs = binary_operation.rhs,
                        .rhs = binary_operation.lhs,
                    }};
        case BinaryOperator::Geq:
            return {
                BinaryOperatorCall{
                    .type = BinaryOperator::Leq,
                    .lhs = binary_operation.rhs,
                    .rhs = binary_operation.lhs,
                }};
        case BinaryOperator::Eq:
            return {BinaryOperatorCall{
                .type = BinaryOperator::Eq,
                .lhs = binary_operation.rhs,
                .rhs = binary_operation.lhs,
            }};
        case BinaryOperator::Neq:
            return {BinaryOperatorCall{
                .type = BinaryOperator::Neq,
                .lhs = binary_operation.rhs,
                .rhs = binary_operation.lhs,
            }};
        case BinaryOperator::And:
            return {
                *binary_operation.lhs,
                *binary_operation.rhs,
                BinaryOperatorCall{
                    .type = BinaryOperator::And,
                    .lhs = binary_operation.rhs,
                    .rhs = binary_operation.lhs,
                }};
        case BinaryOperator::Or:
            return {BinaryOperatorCall{
                .type = BinaryOperator::Or,
                .lhs = binary_operation.rhs,
                .rhs = binary_operation.lhs,
            }};
        case BinaryOperator::Implies:
            return {BinaryOperatorCall{
                .type = BinaryOperator::Or,
                .lhs = std::make_shared<Expression>(UnaryOperatorCall{
                    .type = UnaryOperator::Neg,
                    .operand = binary_operation.lhs,
                }),
                .rhs = binary_operation.rhs,
            }};
        default:
            return {};
        }
    }

    std::vector<Expression> implied(Expression const &expression)
    {
        return std::visit(
            overloaded{
                [](Literal const &literal)
                { return implied_inner(literal); },
                [](Identifier const &identifier)
                { return implied_inner(identifier); },
                [](UnaryOperatorCall const &unary_operation)
                { return implied_inner(unary_operation); },
                [](BinaryOperatorCall const &binary_operation)
                { return implied_inner(binary_operation); },
            },
            expression);
    }

    std::vector<Derivation> ExpandImpliedRule::apply(Goal const &goal) const
    {
        // Expanding implications is meant for dispatching the final pure goal to replace the prover, so we don't want to expand implications before finishing the heap.
        auto const &precondition_empty = goal.spec.precondition.heap.array_declarations.empty() && goal.spec.precondition.heap.array_declarations.empty() && goal.spec.precondition.heap.pointer_declarations.empty();
        auto const &postcondition_empty = goal.spec.postcondition.heap.array_declarations.empty() && goal.spec.postcondition.heap.array_declarations.empty() && goal.spec.postcondition.heap.pointer_declarations.empty();
        if (!precondition_empty || !postcondition_empty)
        {
            return {};
        }
        std::vector<Expression> added_clauses;
        for (auto const &precondition : goal.spec.precondition.proposition)
        {
            for (auto const &clause : implied(precondition))
            {
                if (std::find(goal.spec.precondition.proposition.begin(), goal.spec.precondition.proposition.end(), clause) == goal.spec.precondition.proposition.end())
                {
                    added_clauses.push_back(clause);
                }
            }
        }
        if (added_clauses.empty())
        {
            return {};
        }
        std::vector<Derivation> derivations;
        auto new_goal(goal);
        for (auto const &clause : added_clauses)
        {
            new_goal.spec.precondition.proposition.push_back(clause);
        }
        derivations.push_back(Derivation{
            .goals = {new_goal},
            .continuation = std::make_unique<IdentityContinuation>(),
        });
        return derivations;
    }
}
