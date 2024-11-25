#include <decision_forest_heuristic.hpp>
#include <goal_heuristic_search.hpp>

#include <cstdio>
#include <iostream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <charconv>

struct pclose_deleter
{
    void operator()(FILE *f) const
    {
        pclose(f);
    }
};

std::string exec(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, pclose_deleter> pipe(popen(cmd, "r"), pclose_deleter());
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

namespace desyl
{
    std::string json_inputs(FunctionSpecification const &spec, std::string const &rule)
    {
        std::stringstream ss;
        ss << "\"{";
        ss << "\\\"pre_pures\\\": " << std::to_string(spec.precondition.proposition.size()) << ",";
        ss << "\\\"pre_pointers\\\": " << std::to_string(spec.precondition.heap.pointer_declarations.size()) << ",";
        ss << "\\\"pre_arrays\\\": " << std::to_string(spec.precondition.heap.array_declarations.size()) << ",";
        ss << "\\\"pre_predicates\\\": " << std::to_string(spec.precondition.heap.predicate_calls.size()) << ",";
        ss << "\\\"post_pures\\\": " << std::to_string(spec.postcondition.proposition.size()) << ",";
        ss << "\\\"post_pointers\\\": " << std::to_string(spec.postcondition.heap.pointer_declarations.size()) << ",";
        ss << "\\\"post_arrays\\\": " << std::to_string(spec.postcondition.heap.array_declarations.size()) << ",";
        ss << "\\\"post_predicates\\\": " << std::to_string(spec.postcondition.heap.predicate_calls.size()) << ",";
        ss << "\\\"rule\\\": \\\"" << rule << "\\\"";
        ss << "}\"";
        return ss.str();
    }

    double decision_forest_score(ProofTreeNode const &node)
    {
        std::string cmd = "python run_dt.py " + json_inputs(node.goal->spec, get_previous_rule(node)) + " 2>/dev/null";
        auto result = exec(cmd.c_str());
        double score;
        std::from_chars(result.data(), result.data() + result.size(), score);
        return score;
    }
}
