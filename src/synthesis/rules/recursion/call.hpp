#pragma once

#include <rules.hpp>

namespace desyl
{
    class CallContinuation : public Continuation
    {
        Identifier function_name;
        std::vector<Expression> args;

    public:
        CallContinuation(Identifier function_name, std::vector<Expression> args) : function_name(function_name), args(args){};
        Program join(std::vector<Program> const &results) const;
    };

    class CallRule : public Rule
    {
    public:
        bool is_invertible() const { return false; };
        std::vector<Derivation> apply(Goal const &goal) const;
    };
}
