#include <desyl/ast.hpp>
#include <rules.hpp>
#include <unordered_map>

namespace desyl
{
    Expression substitute_expression(Expression const &expression, Identifier const &identifier, Expression const &substitution);

    using Substitutions = std::unordered_map<Identifier, Expression>;

    bool subtitutions_conflict(Substitutions const &first, Substitutions const &second);

    Expression substitute(Expression const &expression, Substitutions const &substitutions);

    Goal substitute(Goal const &goal, Substitutions const &substitutions);
}
