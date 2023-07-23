#include <set_size.hpp>

#include <failure.hpp>
#include <desyl/ast.hpp>
#include <iostream>

namespace desyl
{
    int maximum_size(BinaryOperatorCall const &call)
    {
        if (!std::holds_alternative<SetLiteral>(*call.rhs))
        {
            return -1;
        }
        auto const &set = std::get<SetLiteral>(*call.rhs);
        if (call.type == BinaryOperator::Iso || call.type == BinaryOperator::Subs)
        {
            return set.elements.size();
        }
        else if (call.type == BinaryOperator::PSubs)
        {
            return set.elements.size() - 1;
        }
        return -1;
    }

    int minimum_size(BinaryOperatorCall const &call)
    {
        if (!std::holds_alternative<SetLiteral>(*call.rhs))
        {
            return -1;
        }
        auto const &set = std::get<SetLiteral>(*call.rhs);
        if (call.type == BinaryOperator::Iso || call.type == BinaryOperator::Sups)
        {
            return set.elements.size();
        }
        else if (call.type == BinaryOperator::PSups)
        {
            return set.elements.size() + 1;
        }
        return -1;
    }

    bool incompatible(Proposition as, Proposition bs)
    {
        for (auto const &a : as)
        {
            if (!std::holds_alternative<BinaryOperatorCall>(a))
            {
                continue;
            }
            auto const &a_call = std::get<BinaryOperatorCall>(a);
            for (auto const &b : bs)
            {
                if (!std::holds_alternative<BinaryOperatorCall>(b))
                {
                    continue;
                }
                auto const &b_call = std::get<BinaryOperatorCall>(b);
                if (*a_call.lhs != *b_call.lhs)
                {
                    continue;
                }
                auto const &a_max_size = maximum_size(a_call);
                auto const &a_min_size = minimum_size(a_call);
                auto const &b_max_size = maximum_size(b_call);
                auto const &b_min_size = minimum_size(b_call);
                if (a_max_size != -1 && b_min_size != -1 && a_max_size < b_min_size)
                {
                    return true;
                }
                if (a_min_size != -1 && b_max_size != -1 && a_min_size > b_max_size)
                {
                    return true;
                }
            }
        }
        return false;
    }

    std::vector<Derivation> SetSizeRule::apply(Goal const &goal) const
    {
        if (incompatible(goal.spec.precondition.proposition, goal.spec.precondition.proposition))
        {
            throw Failure();
        }
        if (incompatible(goal.spec.precondition.proposition, goal.spec.postcondition.proposition))
        {
            throw Failure();
        }
        if (incompatible(goal.spec.postcondition.proposition, goal.spec.precondition.proposition))
        {
            throw Failure();
        }
        if (incompatible(goal.spec.postcondition.proposition, goal.spec.postcondition.proposition))
        {
            throw Failure();
        }
        return {};
    }
}
