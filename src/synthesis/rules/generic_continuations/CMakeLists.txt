add_library(generic_continuations generic_continuations.cpp)

target_include_directories(generic_continuations
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE ..
    PRIVATE .
)
