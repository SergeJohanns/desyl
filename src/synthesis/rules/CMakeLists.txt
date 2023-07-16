add_library(rules rules.cpp emp.cpp frame.cpp write.cpp read.cpp heap_unify.cpp pick.cpp pure_unify.cpp subst_right.cpp generic_continuations.cpp substitution.cpp unification.cpp)
add_subdirectory(pure_rewrites)

target_link_libraries(rules PRIVATE pure_rewrites)

target_include_directories(rules
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE .
)
