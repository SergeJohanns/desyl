add_library(generic_continuations empty_continuation.cpp identity_continuation.cpp)

target_include_directories(generic_continuations
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE ..
    PRIVATE .
)
