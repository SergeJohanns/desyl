#include <iostream>
#include <desyl/desyllib.hpp>
#include <desyl/ast.hpp>

std::string swap = "{true; <x,  0> -> a * <y, 0> -> b;} swap(loc x, loc y) {true; <x, 0> -> b * <y, 0> -> a;}";
std::string empt = "{true;emp;} empt() {true;emp;}";

int main()
{
    std::cout << desyl::example("!f => 4 == 5  < (-54 <= !-4)") << std::endl;
    // desyl::Query query = desyl::parse(swap);
    desyl::Query query = desyl::parse(empt);
    desyl::synthesize(std::move(query));
    return 0;
}
