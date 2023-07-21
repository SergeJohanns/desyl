add_library(predicates open.cpp)

target_include_directories(predicates
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE ..
    PRIVATE .
)
