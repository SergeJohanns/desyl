#include <iostream>
#include <desyl/desyllib.hpp>
#include <desyl/ast.hpp>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: desyl [-v] <query-file>" << std::endl;
        return 1;
    }
    bool verbose = argc == 3 && std::string(argv[1]) == "-v";
    desyl::Query query = desyl::parse(argv[argc - 1]);
    desyl::synthesize(std::move(query), verbose);
    return 0;
}
