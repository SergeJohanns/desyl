#include <desyl/desyllib.hpp>

#include <desyl/ast.hpp>
#include <grammar.hpp>
#include <synthesis.hpp>

namespace desyl
{
    Query parse(std::string const &input)
    {
        return parse_query(input);
    }

    void synthesize(Query query)
    {
        synthesize_query(Goal(std::move(query.functions[0]), std::move(query.predicates)));
    }

    std::string
    example(std::string const &input)
    {
        return stringify_expression(input);
    }
}
