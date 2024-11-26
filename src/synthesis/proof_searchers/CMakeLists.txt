add_library(proof_searchers proof_tree.cpp depth_first.cpp breadth_first.cpp whole_tree.cpp guided.cpp goal_heuristic_search.cpp rule_heuristic_search.cpp priority_queue.cpp suslik_cost.cpp rule_heuristic.cpp decision_forest_heuristic.cpp)

target_include_directories(proof_searchers
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE ../rules
    PRIVATE .
)
