#include <rules.hpp>

#include <desyl/ast.hpp>
#include <synthesis.hpp>
#include <overload.hpp>
#include <vector>
#include <memory>
#include <algorithm>
#include <sstream>
#include <iostream>

namespace desyl
{
    Goal::Goal(FunctionSpecification spec)
    {
        vars(spec.signature, environment);
        this->spec = std::move(spec);
    }

    VariableSnapshot Goal::variables() const
    {
        Vars precondition, postcondition;
        vars(spec.precondition, precondition);
        vars(spec.postcondition, postcondition);
        return VariableSnapshot{
            .precondition = std::move(precondition),
            .environment = std::move(environment),
            .postcondition = std::move(postcondition),
        };
    }

    Vars VariableSnapshot::all()
    {
        Vars result;
        result.insert(precondition.begin(), precondition.end());
        result.insert(environment.begin(), environment.end());
        result.insert(postcondition.begin(), postcondition.end());
        return result;
    }

    Vars VariableSnapshot::ghosts()
    {
        Vars result;
        std::set_difference(
            precondition.begin(), precondition.end(),
            environment.begin(), environment.end(),
            std::inserter(result, result.end()));
        return result;
    }

    Vars VariableSnapshot::existentials()
    {
        // We need to union the signature and precondition separately because reusing the result
        // as both input and output is UB
        Vars exclusion, result;
        std::set_union(
            precondition.begin(), precondition.end(),
            environment.begin(), environment.end(),
            std::inserter(exclusion, exclusion.end()));
        std::set_difference(
            postcondition.begin(), postcondition.end(),
            exclusion.begin(), exclusion.end(),
            std::inserter(result, result.end()));
        return result;
    }

    Program ConstantContinuation::join(std::vector<Program> const &) const
    {
        return program;
    }

    std::vector<Derivation> EmpRule::apply(Goal const &goal) const
    {
        auto const &precondition_empty = std::move(goal).spec.precondition.heap.pointer_declarations.size() == 0;
        auto const &postcondition_empty = std::move(goal).spec.postcondition.heap.pointer_declarations.size() == 0;
        if (precondition_empty && postcondition_empty)
        {
            std::cout << "Using EMP" << std::endl;
            auto deriv = Derivation{
                .goals = std::vector<Goal>{},
                .continuation = std::make_unique<ConstantContinuation>(ConstantContinuation("emp;\n")),
            };
            std::vector<Derivation> result;
            result.push_back(std::move(deriv));
            return result;
        }
        else
        {
            return std::vector<Derivation>{};
        }
    }

    Program IdentityContinuation::join(std::vector<Program> const &result) const
    {
        return result[0];
    }

    template <typename HeapElement>
    void remove_frame_overlap(Vars const &existentials, std::vector<HeapElement> &precondition, std::vector<HeapElement> &postcondition)
    {
        std::vector<std::pair<size_t, size_t>> overlap;
        for (size_t i = 0; i < precondition.size(); ++i)
        {
            for (size_t j = 0; j < postcondition.size(); ++j)
            {
                if (precondition[i] == postcondition[j])
                {
                    overlap.push_back(std::make_pair(i, j));
                }
            }
        }
        std::vector<size_t> precondition_to_remove, postcondition_to_remove;
        for (auto [precondition_index, postcondition_index] : overlap)
        {
            Vars term_vars, intersection;
            vars(precondition[precondition_index], term_vars);
            std::set_intersection(
                term_vars.begin(), term_vars.end(),
                existentials.begin(), existentials.end(),
                std::inserter(intersection, intersection.end()));
            if (intersection.size() != 0)
            {
                continue;
            }
            precondition_to_remove.push_back(precondition_index);
            postcondition_to_remove.push_back(postcondition_index);
        }
        std::sort(precondition_to_remove.begin(), precondition_to_remove.end(), std::greater());
        std::sort(postcondition_to_remove.begin(), postcondition_to_remove.end(), std::greater());
        for (auto precondition_index : precondition_to_remove)
        {
            precondition.erase(precondition.begin() + precondition_index);
        }
        for (auto postcondition_index : postcondition_to_remove)
        {
            postcondition.erase(postcondition.begin() + postcondition_index);
        }
    }

    std::vector<Derivation> FrameRule::apply(Goal const &goal) const
    {
        auto new_goal(goal);
        auto const existentials = new_goal.variables().existentials();

        auto &precondition_pointers = new_goal.spec.precondition.heap.pointer_declarations;
        auto &postcondition_pointers = new_goal.spec.postcondition.heap.pointer_declarations;
        size_t pointer_size = precondition_pointers.size();
        remove_frame_overlap(existentials, precondition_pointers, postcondition_pointers);
        bool pointer_changed = pointer_size != precondition_pointers.size();

        auto &precondition_arrays = new_goal.spec.precondition.heap.array_declarations;
        auto &postcondition_arrays = new_goal.spec.postcondition.heap.array_declarations;
        size_t array_size = precondition_arrays.size();
        remove_frame_overlap(existentials, precondition_arrays, postcondition_arrays);
        bool array_changed = array_size != precondition_arrays.size();

        auto &precondition_predicates = new_goal.spec.precondition.heap.predicate_calls;
        auto &postcondition_predicates = new_goal.spec.postcondition.heap.predicate_calls;
        size_t predicate_size = precondition_predicates.size();
        remove_frame_overlap(existentials, precondition_predicates, postcondition_predicates);
        bool predicate_changed = predicate_size != precondition_predicates.size();

        if (!pointer_changed && !array_changed && !predicate_changed)
        {
            return std::vector<Derivation>{};
        }

        std::cout << "Using FRAME" << std::endl;
        Derivation deriv{
            .goals = std::vector<Goal>{new_goal},
            .continuation = std::make_unique<IdentityContinuation>(IdentityContinuation()),
        };
        std::vector<Derivation> result;
        result.push_back(std::move(deriv));
        return result;
    }

    WriteContinuation::WriteContinuation(PointerDeclaration const &pointer)
    {
        std::ostringstream stream;
        stream << "*(" << stringify_expression(*pointer.base) << " + " << pointer.offset << ") = " << stringify_expression(*pointer.value) << ";" << std::endl;
        program = stream.str();
    }

    Program WriteContinuation::join(std::vector<Program> const &result) const
    {
        return program + result[0];
    }

    std::vector<Derivation> WriteRule::apply(Goal const &goal) const
    {
        std::vector<Derivation> result;
        auto &preconditions = goal.spec.precondition.heap.pointer_declarations;
        auto &postconditions = goal.spec.postcondition.heap.pointer_declarations;
        for (size_t i = 0; i < preconditions.size(); ++i)
        {
            for (size_t j = 0; j < postconditions.size(); ++j)
            {
                if (*preconditions[i].base == *postconditions[j].base && preconditions[i].offset == postconditions[j].offset && *preconditions[i].value != *postconditions[j].value)
                {
                    Vars term_vars;
                    auto environment = goal.variables().environment;
                    vars(*postconditions[j].value, term_vars);
                    if (!std::includes(
                            environment.begin(), environment.end(),
                            term_vars.begin(), term_vars.end()))
                    {
                        continue;
                    }
                    std::cout << "Using WRITE" << std::endl;
                    auto new_goal(goal);
                    auto new_declaration = PointerDeclaration{
                        .base = preconditions[i].base,
                        .offset = preconditions[i].offset,
                        .value = postconditions[j].value,
                    };
                    new_goal.spec.precondition.heap.pointer_declarations[i] = new_declaration;
                    Derivation deriv{
                        .goals = std::vector<Goal>{new_goal},
                        .continuation = std::make_unique<WriteContinuation>(WriteContinuation(new_declaration)),
                    };
                    result.push_back(std::move(deriv));
                }
            }
        }
        return result;
    }

    ReadContinuation::ReadContinuation(Identifier const &output, PointerDeclaration const &pointer)
    {
        std::ostringstream stream;
        stream << output << " = *(" << stringify_expression(*pointer.base) << " + " << pointer.offset << ");" << std::endl;
        program = stream.str();
    }

    Program ReadContinuation::join(std::vector<Program> const &result) const
    {
        return program + result[0];
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
        target.proposition = substitute(target.proposition, before, after);
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
            std::cout << "Using READ" << std::endl;
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
