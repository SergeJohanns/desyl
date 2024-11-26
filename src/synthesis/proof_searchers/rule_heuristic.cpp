#include <rule_heuristic.hpp>
#include <rule_heuristic_search.hpp>

#include <all_rules.hpp>
#include <unordered_map>

namespace desyl
{
    std::unordered_map<std::string, int> get_heuristic_map()
    {
        std::unordered_map<std::string, int> heuristic_map;
        int i = 1;
        for (auto const &rule : all_rules())
        {
            heuristic_map[rule->name()] = i;
            i++;
        }
        return heuristic_map;
    }

    std::unordered_map<std::string, int> heuristic_map = get_heuristic_map();

    int rule_heuristic(ProofTreeNode const &, std::string const &rule)
    {
        return heuristic_map[rule];
    }
}
