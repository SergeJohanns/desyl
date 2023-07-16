add_library(failures post_invalid.cpp pre_inconsistent.cpp)

target_include_directories(failures
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE ..
    PRIVATE .
)
