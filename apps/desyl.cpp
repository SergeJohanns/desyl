#include <desyl/desyllib.hpp>

#include <iostream>

int main()
{
    std::cout << desyl::example("!f == 5 < (-54 <= !-4)") << std::endl;
    return 0;
}
