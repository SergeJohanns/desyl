#include <synthesis.hpp>

#include <iostream>
#include <optional>
#include <desyl/ast.hpp>

namespace desyl
{
    // Pointers because the subclasses take up different amounts of memory
    const std::unique_ptr<Rule> all_rules[RULES] = {
        std::make_unique<EmpRule>(EmpRule()),
        std::make_unique<FrameRule>(FrameRule()),
        std::make_unique<WriteRule>(WriteRule()),
    };

    std::optional<Program> solve_subgoals(std::vector<Goal> const &goals, Continuation const &continuation)
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

    std::optional<Program> try_alts(std::vector<Derivation> const &subderivs, std::unique_ptr<Rule> const &)
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

    std::optional<Program> with_rules(std::unique_ptr<Rule> const (&rules)[RULES], Goal const &goal)
    {
        for (auto &rule : rules)
        {
            auto sub = try_alts(rule->apply(std::move(goal)), rule);
            if (sub.has_value())
            {
                return sub.value();
            }
        }
        return std::nullopt;
    }

    void synthesize_query(Goal const &spec)
    {
        auto res = with_rules(all_rules, spec);
        if (res.has_value())
        {
            std::cout << res.value() << std::endl;
        }
        else
        {
            std::cout << "No value" << std::endl;
        }
    }
}
