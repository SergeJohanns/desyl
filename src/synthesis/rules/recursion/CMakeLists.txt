add_library(recursion open.cpp close.cpp call.cpp)

target_include_directories(recursion
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE ..
    PRIVATE .
    PRIVATE ../substitution
)
