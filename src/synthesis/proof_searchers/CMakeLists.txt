add_library(proof_searchers proof_tree.cpp depth_first.cpp breadth_first.cpp whole_tree.cpp guided.cpp heuristic_search.cpp suslik_cost.cpp rule_heuristic.cpp)

target_include_directories(proof_searchers
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE ../rules
    PRIVATE .
)
