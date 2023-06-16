#include <iostream>
#include <desyl/desyllib.hpp>
#include <desyl/ast.hpp>

std::string empt = "{true;emp;} empt() {true;emp;}";
std::string fram = "{true; <x, 0> -> a;} frame() {true; <x, 0> -> a;}";
std::string writ = "{true; <x, 0> -> a;} frame(int b) {true; <x, 0> -> b;}";
std::string swap = "{true; <x,  0> -> a * <y, 0> -> b;} swap(loc x, loc y) {true; <x, 0> -> b * <y, 0> -> a;}";

int main()
{
    desyl::Query query = desyl::parse(swap);
    desyl::synthesize(std::move(query));
    return 0;
}
