add_library(pure_rewrites pure_frame.cpp reflexivity.cpp expand_implied.cpp true_elision.cpp null_not_lval.cpp)

target_include_directories(pure_rewrites
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE ..
    PRIVATE .
)
