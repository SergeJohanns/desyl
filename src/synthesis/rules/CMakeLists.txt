add_library(rules rules.cpp emp.cpp frame.cpp generic_continuations.cpp)
add_subdirectory(statements)
add_subdirectory(pure_rewrites)
add_subdirectory(failures)
add_subdirectory(substitution)

target_link_libraries(rules PRIVATE statements pure_rewrites failures substitution)

target_include_directories(rules
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE .
)
