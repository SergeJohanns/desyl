#include <synthesis.hpp>

#include <iostream>
#include <optional>
#include <desyl/ast.hpp>

namespace desyl
{
    auto all_rules = std::vector<Rule>{};

    std::optional<Program> solve_subgoals(std::vector<Context> const &goals, Continuation const &continuation)
    {
        auto result = std::vector<Program>{};
        for (auto const &goal : goals)
        {
            auto sub = with_rules(all_rules, std::move(goal));
            if (!sub.has_value())
            {
                return std::nullopt;
            }
            result.push_back(sub.value());
        }
        return continuation.join(result);
    }

    std::optional<Program> try_alts(std::vector<Derivation> const &subderivs, Rule const &)
    {
        for (auto const &deriv : subderivs)
        {
            auto sub = solve_subgoals(deriv.goals, *deriv.continuation);
            if (sub.has_value())
            {
                return sub.value();
            }
        }
        return std::nullopt;
    }

    std::optional<Program> with_rules(std::vector<Rule> const &rules, Context const &goal)
    {
        for (auto &rule : rules)
        {
            auto sub = try_alts(rule.apply(std::move(goal)), rule);
            if (sub.has_value())
            {
                return sub.value();
            }
        }
        return std::nullopt;
    }

    void synthesize_query(Context const &spec)
    {
        auto res = with_rules(all_rules, spec);
        if (res.has_value())
        {
            std::cout << res.value() << std::endl;
        }
    }

    Program ConstantContinuation::join(std::vector<Program> const &) const
    {
        return program;
    }

    std::vector<Derivation> EmpRule::apply(Context const &context) const
    {
        auto const &precondition_empty = std::move(context).precondition.heap.pointer_declarations.size() == 0;
        auto const &postcondition_empty = std::move(context).postcondition.heap.pointer_declarations.size() == 0;
        if (precondition_empty && postcondition_empty)
        {
            auto deriv = Derivation{
                .goals = std::vector<Context>{},
                .continuation = std::make_unique<ConstantContinuation>(ConstantContinuation("emp")),
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
}
