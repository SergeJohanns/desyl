#include <iostream>
#include <desyl/desyllib.hpp>
#include <desyl/ast.hpp>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: desyl [-v | -g] <query-file>" << std::endl;
        return 1;
    }
    desyl::SynthesisMode mode = desyl::SynthesisMode::Quiet;
    if (argc == 3)
    {
        if (std::string(argv[1]) == "-v")
        {
            mode = desyl::SynthesisMode::Verbose;
        }
        else if (std::string(argv[1]) == "-g")
        {
            mode = desyl::SynthesisMode::Guided;
        }
        else
        {
            std::cout << "Usage: desyl [-v | -g] <query-file>" << std::endl;
            return 1;
        }
    }
    desyl::Query query = desyl::parse(argv[argc - 1]);
    desyl::synthesize(std::move(query), mode);
    return 0;
}
