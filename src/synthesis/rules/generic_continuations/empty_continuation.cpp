#include <empty_continuation.hpp>

#include <sstream>

namespace desyl
{
    Program EmptyContinuation::join(std::vector<Program> const &) const
    {
        return Program{""};
    }
}
