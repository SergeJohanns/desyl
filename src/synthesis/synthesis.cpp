#include <synthesis.hpp>

#include <iostream>
#include <optional>
#include <desyl/ast.hpp>
#include <rules.hpp>
#include <all_rules.hpp>

namespace desyl
{
    // Forward declaration (no header) because the function is not public but is part of mutual recursion
    std::optional<Program> with_rules(std::unique_ptr<Rule> const (&rules)[RULES], Goal const &goal);

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
        std::cout << std::endl;
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
