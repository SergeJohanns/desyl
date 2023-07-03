#include <iostream>
#include <desyl/desyllib.hpp>
#include <desyl/ast.hpp>

std::string empt = "{true;emp;} empt() {true;emp;}";
std::string fram = "{true; <x, 0> -> a;} frame() {true; <x, 0> -> a;}";
std::string writ = "{true; <x, 0> -> a;} frame(int b) {true; <x, 0> -> b;}";
std::string swap = "{true; <x,  0> -> a * <y, 0> -> b;} swap(loc x, loc y) {true; <x, 0> -> b * <y, 0> -> a;}";
std::string three_swap = "{true; <x,  0> -> a * <y, 0> -> b * <z, 0> -> c;} swap(loc x, loc y, loc z) {true; <x, 0> -> b * <y, 0> -> c * <z, 0> -> a;}";
std::string four_swap = "{true; <x,  0> -> a * <y, 0> -> b * <z, 0> -> c * <zz, 0> -> d;} swap(loc x, loc y, loc z, loc zz) {true; <x, 0> -> b * <y, 0> -> c * <z, 0> -> d * <zz, 0> -> a;}";
// Requires unifyheaps
std::string pick = "{true; <x,  0> -> 239 * <y, 0> -> 30;} pick(loc x, loc y) {true; <x, 0> -> z * <y, 0> -> z;}";
std::string notsure = "{true; <x,  0> -> a * <y, 0> -> b;} notsure(loc x, loc y) {true; <x, 0> -> c * <c, 0> -> 0;}";

int main()
{
    desyl::Query query = desyl::parse(notsure);
    desyl::synthesize(std::move(query));
    return 0;
}
