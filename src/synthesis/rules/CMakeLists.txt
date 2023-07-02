add_library(rules emp.cpp frame.cpp write.cpp read.cpp generic_continuations.cpp)

target_include_directories(rules
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${PROJECT_SOURCE_DIR}/src/include"
    PRIVATE .
)
