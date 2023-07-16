#include <generic_continuations.hpp>

#include <sstream>

namespace desyl
{
    Program IdentityContinuation::join(std::vector<Program> const &result) const
    {
        return result[0];
    }

    void output_variable(std::stringstream &out, TypedVariable const &param)
    {
        if (param.type == Type::Int)
        {
            out << "int";
        }
        else if (param.type == Type::Loc)
        {
            out << "loc";
        }
        else
        {
            throw std::runtime_error("Unknown type");
        }
        out << " " << param.name;
    }

    Program EmptyContinuation::join(std::vector<Program> const &) const
    {
        return Program{""};
    }
}
