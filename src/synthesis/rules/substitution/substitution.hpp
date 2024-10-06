#include <desyl/ast.hpp>
#include <rules.hpp>
#include <unordered_map>

namespace desyl
{
    Expression substitute_expression(Expression const &expression, Identifier const &identifier, Expression const &substitution);

    using Substitutions = std::unordered_map<Identifier, Expression>;

    bool subtitutions_conflict(Substitutions const &first, Substitutions const &second);

    Expression substitute(Expression const &expression, Substitutions const &substitutions);

    Proposition substitute(Proposition const &proposition, Substitutions const &substitutions);

    PointerDeclaration substitute(PointerDeclaration const &pointer, Substitutions const &substitutions);

    ArrayDeclaration substitute(ArrayDeclaration const &heap, Substitutions const &substitutions);

    PredicateCall substitute(PredicateCall const &heap, Substitutions const &substitutions);

    Heap substitute(Heap const &heap, Substitutions const &substitutions);

    Assertion substitute(Assertion const &assertion, Substitutions const &substitutions);

    Goal substitute(Goal const &goal, Substitutions const &substitutions);
}
