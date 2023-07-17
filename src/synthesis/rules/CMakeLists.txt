add_library(rules rules.cpp frame.cpp)
add_subdirectory(generic_continuations)
add_subdirectory(statements)
add_subdirectory(heap_rewrites)
add_subdirectory(pure_rewrites)
add_subdirectory(failures)
add_subdirectory(substitution)

target_link_libraries(rules PRIVATE generic_continuations statements heap_rewrites pure_rewrites failures substitution)

target_include_directories(rules
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE .
)
