#pragma once

#include <desyl/ast.hpp>
#include <set>

namespace desyl
{
    using Vars = std::set<Identifier>;

    void vars(Literal const &element, Vars &target);

    void vars(Identifier const &element, Vars &target);

    void vars(UnaryOperatorCall const &element, Vars &target);

    void vars(BinaryOperatorCall const &element, Vars &target);

    void vars(Expression const &element, Vars &target);

    void vars(ArrayDeclaration const &element, Vars &target);

    void vars(PointerDeclaration const &element, Vars &target);

    void vars(PredicateCall const &element, Vars &target);

    void vars(Heap const &element, Vars &target);

    void vars(Assertion const &element, Vars &target);

    void vars(TypedVariable const &element, Vars &target);

    void vars(FunctionSignature const &element, Vars &target);

    void vars(Clause const &element, Vars &target);

    void vars(Predicate const &element, Vars &target);

    Identifier rename(Identifier const &element, Vars const &environment);
}
