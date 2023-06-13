#include <rules.hpp>

#include <vector>
#include <memory>

namespace desyl
{

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
