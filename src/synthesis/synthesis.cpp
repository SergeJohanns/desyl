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
    template <typename RULES>
    std::optional<Program> with_rules(RULES const &rules, Goal const &goal, bool verbose);

    /// @brief Try to synthesize programs for a list of subgoals and join them
    /// @tparam RULES The rules array to use for each subgoal
    /// @param goals The subgoals to synthesize
    /// @param continuation The continuation to join the synthesized programs
    /// @param rules The rules array to use for each subgoal
    /// @param verbose Whether to print debug information
    /// @return The synthesized program, if any
    template <typename RULES>
    std::optional<Program> solve_subgoals(std::vector<Goal> const &goals, Continuation const &continuation, RULES const &rules, bool verbose)
    {
        auto result = std::vector<Program>{};
        for (auto const &goal : goals)
        {
            auto sub = with_rules(rules, std::move(goal), verbose);
            if (!sub.has_value())
            {
                return std::nullopt;
            }
            result.push_back(sub.value());
        }
        return continuation.join(result);
    }

    /// @brief Try to synthesize a program from a list of subderivations of a rule
    /// @tparam RULES The rules array to use for each derivation
    /// @param subderivs The subderivations to try
    /// @param rule The rule that was applied to get the subderivations
    /// @param rules The rules array to use for each derivation
    /// @param verbose Whether to print debug information
    /// @return The synthesized program, if any
    template <typename RULES>
    std::optional<Program> try_alts(std::vector<Derivation> const &subderivs, std::unique_ptr<Rule> const &, RULES const &rules, bool verbose)
    {
        for (auto const &deriv : subderivs)
        {
            auto sub = solve_subgoals(deriv.goals, *deriv.continuation, rules, verbose);
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
    /// @param verbose Whether to print debug information
    /// @return The synthesized program, if any
    template <typename RULES>
    std::optional<Program> with_rules(RULES const &rules, Goal const &goal, bool verbose)
    {
        for (auto &rule : rules)
        {
            std::vector<Derivation> subderivs;
            try
            {
                subderivs = rule->apply(std::move(goal));
            }
            catch (Failure const &)
            {
                if (verbose)
                {
                    std::cout << "Using " << rule->name() << std::endl;
                }
                return std::nullopt;
            }
            if (subderivs.empty())
            {
                continue;
            }
            if (verbose)
            {
                std::cout << "Using " << rule->name() << std::endl;
            }
            // if (verbose)
            // {
            //     for (auto const &deriv : subderivs)
            //     {
            //         for (auto const &goal : deriv.goals)
            //         {
            //             std::cout << stringify_function_spec(goal.spec) << std::endl;
            //         }
            //     }
            // }
            auto sub = try_alts(subderivs, rule, rules, verbose);
            if (sub.has_value())
            {
                return sub.value();
            }
            if (verbose)
            {
                std::cout << "Backtracking from " << rule->name() << std::endl;
            }
            if (rule->is_invertible())
            {
                return std::nullopt;
            }
        }
        return std::nullopt;
    }

    /// @brief Synthesize a function from a specification
    /// @param spec The specification to synthesize
    /// @param verbose Whether to print debug information
    void synthesize_query(Goal const &spec, bool verbose)
    {
        auto res = with_rules(all_rules, spec, verbose);
        if (verbose)
        {
            std::cout << std::endl;
        }
        if (res.has_value())
        {
            std::cout << "void " << spec.spec.signature.name << "(";
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
            std::cout << ") ";
            std::cout << res.value() << std::endl;
        }
        else
        {
            std::cout << "No value" << std::endl;
        }
    }
}
