add_library(heap_rewrites emp.cpp frame.cpp)

target_include_directories(heap_rewrites
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE ..
    PRIVATE .
)
