#include <rule_heuristic.hpp>
#include <goal_heuristic_search.hpp>

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

    int rule_heuristic(ProofTreeNode const &node)
    {
        std::string previous_rule = get_previous_rule(node);
        if (previous_rule == "")
        {
            return 0;
        }
        return heuristic_map[previous_rule];
    }
}