#include <synthesis.hpp>

#include <iostream>
#include <desyl/ast.hpp>

namespace desyl
{
    void synthesize_query(Query const &query)
    {
        std::cout
            << "Synthesizing "
            << query.functions.size()
            << " function"
            << (query.functions.size() == 1 ? "" : "s")
            << std::endl;
    }
}
