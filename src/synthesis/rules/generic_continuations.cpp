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

    ConstantContinuation::ConstantContinuation(FunctionSignature const &signature)
    {
        std::stringstream out;
        out << "void " << signature.name << "(";
        for (size_t i = 0; i < signature.args.size(); ++i)
        {
            output_variable(out, signature.args[i]);
            if (i != signature.args.size() - 1)
            {
                out << ", ";
            }
        }
        out << ")";
        outer = out.str();
    }

    Program ConstantContinuation::join(std::vector<Program> const &) const
    {
        return Program(outer);
    }
}
