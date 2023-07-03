#include <rules.hpp>

namespace desyl
{
    Program Program::add_line(Line const &line) const
    {
        Program out(*this);
        out.code += std::string(indentation_level, '\t') + line;
        return out;
    }

    std::ostream &operator<<(std::ostream &os, const Program &program)
    {
        os << program.outer << "\n";
        os << "{\n";
        os << program.code;
        os << "}\n";
        return os;
    }
}
