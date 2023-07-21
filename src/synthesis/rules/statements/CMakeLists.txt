add_library(statements read.cpp write.cpp branch.cpp free.cpp)

target_include_directories(statements
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE ..
    PRIVATE .
)
