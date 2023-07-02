#include <generic_continuations.hpp>

namespace desyl
{
    Program IdentityContinuation::join(std::vector<Program> const &result) const
    {
        return result[0];
    }

    Program ConstantContinuation::join(std::vector<Program> const &) const
    {
        return program;
    }
}
