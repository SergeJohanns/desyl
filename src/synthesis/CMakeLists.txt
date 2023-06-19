add_library(synthesis synthesis.cpp goal.cpp)
add_subdirectory(rules)

target_link_libraries(synthesis PRIVATE context rules)

target_include_directories(synthesis
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE rules
)
