add_library(synthesis synthesis.cpp goal.cpp)
add_subdirectory(rules)
add_subdirectory(proof_searchers)

target_link_libraries(synthesis PRIVATE context proof_searchers rules) # proof_searchers has to be before rules to avoid linking errors

target_include_directories(synthesis
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE rules
    PRIVATE proof_searchers
    PRIVATE .
)
