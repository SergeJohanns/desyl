#include <iostream>
#include <desyl/desyllib.hpp>
#include <desyl/ast.hpp>

int main()
{
    std::cout << desyl::example("!f => 4 == 5  < (-54 <= !-4)") << std::endl;
    desyl::Query query = desyl::parse("{true; <x,  0> -> a * <y, 0> -> b;} swap(loc x, loc y) {true; <x, 0> -> b * <y, 0> -> a;}");
    return 0;
}
