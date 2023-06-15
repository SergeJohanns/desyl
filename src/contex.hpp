#pragma once

#include <desyl/ast.hpp>
#include <set>

namespace desyl
{
    using Vars = std::set<Identifier>;

    Vars vars(Literal const &element, Vars target);

    Vars vars(Identifier const &element, Vars target);

    Vars vars(UnaryOperatorCall const &element, Vars target);

    Vars vars(BinaryOperatorCall const &element, Vars target);

    Vars vars(Expression const &element, Vars target);

    Vars vars(ArrayDeclaration const &element, Vars target);

    Vars vars(PointerDeclaration const &element, Vars target);

    Vars vars(PredicateCall const &element, Vars target);

    Vars vars(Heap const &element, Vars target);

    Vars vars(Assertion const &element, Vars target);

    Vars vars(TypedVariable const &element, Vars target);

    Vars vars(FunctionSignature const &element, Vars target);
}
