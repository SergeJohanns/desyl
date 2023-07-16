#include <iostream>
#include <desyl/desyllib.hpp>
#include <desyl/ast.hpp>

// Requires emp
std::string empty = "{true;emp;} empty() {true;emp;}";
// Requires frame
std::string frame = "{true; <x, 0> -> a;} frame() {true; <x, 0> -> a;}";
// Requires write
std::string writer = "{true; <x, 0> -> a;} write(int b) {true; <x, 0> -> b;}";
// Requires read
std::string swap = "{true; <x,  0> -> a | <y, 0> -> b;} swap(loc x, loc y) {true; <x, 0> -> b | <y, 0> -> a;}";
std::string three_swap = "{true; <x,  0> -> a | <y, 0> -> b | <z, 0> -> c;} swap(loc x, loc y, loc z) {true; <x, 0> -> b | <y, 0> -> c | <z, 0> -> a;}";
std::string four_swap = "{true; <x,  0> -> a | <y, 0> -> b | <z, 0> -> c | <zz, 0> -> d;} swap(loc x, loc y, loc z, loc zz) {true; <x, 0> -> b | <y, 0> -> c | <z, 0> -> d | <zz, 0> -> a;}";
// Requires unifyheaps
std::string pick = "{true; <x,  0> -> 239 | <y, 0> -> 30;} pick(loc x, loc y) {true; <x, 0> -> z | <y, 0> -> z;}";
std::string three_pick = "{true; <x,  0> -> 239 | <y, 0> -> 30 | <z, 0> -> 42;} pick(loc x, loc y, loc z) {true; <x, 0> -> n | <y, 0> -> n | <z, 0> -> n;}";
// Requires pick
std::string notsure = "{true; <x,  0> -> a | <y, 0> -> b;} notsure(loc x, loc y) {true; <x, 0> -> c | <c, 0> -> 0;}";
std::string three_notsure = "{true; <x,  0> -> a | <y, 0> -> b | <z, 0> -> null;} notsure(loc x, loc y, loc z) {true; <x, 0> -> c | <c, 0> -> 0 | <z, 0> -> c;}";
// Requires pureunify
std::string elem = "{S == v + R; <x, 0> -> a;} pure(loc x, int v) {S == u + R; <x, 0> -> v1 + 1;}";
// Requires branch?
std::string max = "{true; <r, 0> -> null;} max(loc r, int x, int y) { x <= m && y <= m; <r, 0> -> m;}";
// Shouldn't find any clauses even with branch
std::string three_max = "{true; <r, 0> -> null;} max(loc r, int x, int y) { x <= m && y <= m && z <= m; <r, 0> -> m;}";

int main()
{
    desyl::Query query = desyl::parse(elem);
    desyl::synthesize(std::move(query));
    return 0;
}
