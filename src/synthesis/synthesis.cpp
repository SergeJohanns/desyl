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
    std::optional<Program> solve_subgoals(std::vector<Goal> const &goals, Continuation const &continuation, RULES const &rules, SynthesisMode mode)
    {
        auto result = std::vector<Program>{};
        for (auto const &goal : goals)
        {
            auto sub = with_rules(rules, std::move(goal), mode);
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
    std::optional<Program> try_alts(std::vector<Derivation> const &subderivs, std::unique_ptr<Rule> const &, RULES const &rules, SynthesisMode mode)
    {
        for (auto const &deriv : subderivs)
        {
            auto sub = solve_subgoals(deriv.goals, *deriv.continuation, rules, mode);
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
    std::optional<Program> with_rules(RULES const &rules, Goal const &goal, SynthesisMode mode)
    {
        // Only used for guided synthesis
        std::string chosen_rule;
        if (mode == SynthesisMode::Guided)
        {
            std::cout << "Synthesizing " << stringify_function_spec(goal.spec) << std::endl;
            std::cin >> chosen_rule;
        }
        for (auto &rule : rules)
        {
            if (mode == SynthesisMode::Guided && rule->name() != chosen_rule)
            {
                continue;
            }
            std::vector<Derivation> subderivs;
            try
            {
                subderivs = rule->apply(std::move(goal));
            }
            catch (Failure const &)
            {
                if (mode == SynthesisMode::Verbose)
                {
                    std::cout << "Using " << rule->name() << std::endl;
                }
                break;
            }
            if (subderivs.empty())
            {
                continue;
            }
            if (mode == SynthesisMode::Verbose)
            {
                std::cout << "Using " << rule->name() << std::endl;
            }
            if (mode == SynthesisMode::Guided)
            {
                std::cout << subderivs.size() << " subderivation";
                if (subderivs.size() != 1)
                {
                    std::cout << "s";
                }
                std::cout << std::endl;
            }
            auto sub = try_alts(subderivs, rule, rules, mode);
            if (sub.has_value())
            {
                return sub.value();
            }
            if (mode == SynthesisMode::Verbose)
            {
                std::cout << "Backtracking from " << rule->name() << std::endl;
            }
            if (rule->is_invertible())
            {
                break;
            }
        }
        if (mode == SynthesisMode::Guided)
        {
            std::cout << "Failed, backtracking" << std::endl;
        }
        return std::nullopt;
    }

    /// @brief Synthesize a function from a specification
    /// @param spec The specification to synthesize
    /// @param verbose Whether to print debug information
    void synthesize_query(Goal const &spec, SynthesisMode mode)
    {
        auto res = with_rules(all_rules, spec, mode);
        if (mode != SynthesisMode::Quiet)
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
