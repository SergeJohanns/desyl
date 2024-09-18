#include <iostream>
#include <desyl/desyllib.hpp>
#include <desyl/ast.hpp>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: desyl [-v | -g] [search-algorithm] <query-file>" << std::endl;
        return 1;
    }
    desyl::SynthesisMode mode = desyl::SynthesisMode::Quiet;
    if (argc >= 3 && argv[1][0] == '-')
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
            std::cout << "Usage: desyl [-v | -g] [search-algorithm] <query-file>" << std::endl;
            return 1;
        }
    }
    std::string search_algorithm = "dfs";
    if ((argc == 3 && argv[1][0] != '-') || argc >= 4)
    {
        search_algorithm = argv[argc - 2];
    }
    desyl::Query query = desyl::parse(argv[argc - 1]);
    desyl::synthesize(std::move(query), search_algorithm, mode);
    return 0;
}
