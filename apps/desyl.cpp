#include <iostream>
#include <desyl/desyllib.hpp>
#include <desyl/ast.hpp>
#include <gflags/gflags.h>

bool validate_algo(const char *, const std::string &value)
{
    if (value == "dfs" || value == "bfs" || value == "tree")
    {
        return true;
    }
    return false;
}

bool validate_file(const char *, const std::string &value)
{
    if (value.empty())
    {
        std::cerr << "Query file must be specified" << std::endl;
        return false;
    }
    return true;
}

DEFINE_bool(v, false, "Enable verbose output for synthesis");
DEFINE_bool(g, false, "Enable user guided synthesis");
DEFINE_int32(depth, -1, "Limit the depth of the tree exploration in the search algorithm");
DEFINE_string(algo, "dfs", "Search algorithm to use (dfs, bfs, tree)");
DEFINE_validator(algo, &validate_algo);
DEFINE_string(treeout, "", "File to write the search tree JSON to");
DEFINE_string(file, "", "Path to the file containing the query to synthesize");
DEFINE_validator(file, &validate_file);

int main(int argc, char **argv)
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    desyl::SynthesisMode mode = desyl::SynthesisMode::Quiet;
    if (FLAGS_v)
    {
        mode = desyl::SynthesisMode::Verbose;
    }
    else if (FLAGS_g)
    {
        mode = desyl::SynthesisMode::Guided;
    }
    desyl::Query const query = desyl::parse(FLAGS_file);
    auto const config = desyl::SynthesisConfig{FLAGS_algo, FLAGS_depth, FLAGS_treeout, mode};
    desyl::synthesize(query, config);
    return 0;
}
