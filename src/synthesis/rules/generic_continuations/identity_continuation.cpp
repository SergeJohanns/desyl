#include <identity_continuation.hpp>

#include <sstream>

namespace desyl
{
    Program IdentityContinuation::join(std::vector<Program> const &result) const
    {
        return result[0];
    }
}
