add_library(parser grammar.cpp)

target_link_libraries(parser PRIVATE foonathan::lexy)

target_include_directories(parser
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
)
