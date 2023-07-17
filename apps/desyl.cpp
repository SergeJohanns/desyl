#include <iostream>
#include <desyl/desyllib.hpp>
#include <desyl/ast.hpp>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: desyl <query-file>" << std::endl;
        return 1;
    }
    desyl::Query query = desyl::parse(argv[1]);
    desyl::synthesize(std::move(query));
    return 0;
}
