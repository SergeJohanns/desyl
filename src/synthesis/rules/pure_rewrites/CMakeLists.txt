add_library(pure_rewrites pure_frame.cpp)

target_include_directories(pure_rewrites
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE ..
    PRIVATE .
)
