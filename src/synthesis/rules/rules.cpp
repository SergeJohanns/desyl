#include <rules.hpp>

#include <sstream>

namespace desyl
{
    Program Program::add_lines(Line const &lines) const
    {
        Program out(*this);
        std::stringstream line_iter(lines);
        std::string line;
        std::stringstream prepend;
        while (std::getline(line_iter, line))
        {
            prepend << std::string(indentation_level * 4, ' ') << line << "\n";
        }
        out.code = prepend.str() + out.code;
        return out;
    }

    std::ostream &operator<<(std::ostream &os, const Program &program)
    {
        os << program.outer;
        os << "{\n";
        os << program.code;
        os << "}\n";
        return os;
    }
}
