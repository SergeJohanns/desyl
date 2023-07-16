#include <synthesis.hpp>

#include <iostream>
#include <optional>
#include <desyl/ast.hpp>
#include <rules.hpp>
#include <all_rules.hpp>
#include <failures/failure.hpp>

namespace desyl
{
    // Forward declaration (no header) because the function is not public but is part of mutual recursion
    std::optional<Program> with_rules(std::unique_ptr<Rule> const (&rules)[RULES], Goal const &goal);

    /// @brief Try to synthesize programs for a list of subgoals and join them
    /// @param goals The subgoals to synthesize
    /// @param continuation The continuation to join the synthesized programs
    /// @return The synthesized program, if any
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

    /// @brief Try to synthesize a program from a list of subderivations of a rule
    /// @param subderivs The subderivations to try
    /// @param rule The rule that was applied to get the subderivations
    /// @return The synthesized program, if any
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

    /// @brief Synthesize a program from a specification using a set of rules
    /// @param rules The rules to use
    /// @param goal The specification to synthesize
    /// @return The synthesized program, if any
    std::optional<Program> with_rules(std::unique_ptr<Rule> const (&rules)[RULES], Goal const &goal)
    {
        for (auto &rule : rules)
        {
            std::vector<Derivation> subderivs;
            try
            {
                subderivs = rule->apply(std::move(goal));
            }
            catch (std::exception(Failure))
            {
                return std::nullopt;
            }
            auto sub = try_alts(subderivs, rule);
            if (sub.has_value())
            {
                return sub.value();
            }
        }
        return std::nullopt;
    }

    /// @brief Synthesize a function from a specification
    /// @param spec The specification to synthesize
    void synthesize_query(Goal const &spec)
    {
        auto res = with_rules(all_rules, spec);
        std::cout << std::endl
                  << "void " << spec.spec.signature.name << "(";
        for (auto const &param : spec.spec.signature.args)
        {
            if (param.type == Type::Int)
            {
                std::cout << "int ";
            }
            else if (param.type == Type::Loc)
            {
                std::cout << "loc ";
            }
            std::cout << param.name;
            if (&param != &spec.spec.signature.args.back())
            {
                std::cout << ", ";
            }
        }
        std::cout << ")";
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
