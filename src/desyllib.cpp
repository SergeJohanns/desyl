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
        for (auto const &function : query.functions)
        {
            synthesize_query(Goal(function.second, query.functions, query.predicates));
        }
    }

    std::string
    example(std::string const &input)
    {
        return stringify_expression(input);
    }
}
