add_library(rules rules.cpp)
add_subdirectory(heap_rewrites)
add_subdirectory(generic_continuations)
add_subdirectory(statements)
add_subdirectory(pure_rewrites)
add_subdirectory(failures)
add_subdirectory(substitution)
add_subdirectory(predicates)

# generic_continuations has to be after heap_rewrites to avoid linking errors
target_link_libraries(rules PRIVATE statements heap_rewrites pure_rewrites predicates failures substitution generic_continuations)

target_include_directories(rules
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE .
)
