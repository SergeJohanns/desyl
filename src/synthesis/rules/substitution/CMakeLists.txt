add_library(substitution heap_unify.cpp pick.cpp pure_unify.cpp subst_right.cpp substitution.cpp unification.cpp)

target_include_directories(substitution
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE ..
    PRIVATE .
)
