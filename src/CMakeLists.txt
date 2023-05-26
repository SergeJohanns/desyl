set(HEADER_LIST "${PROJECT_SOURCE_DIR}/include/desyl/desyllib.hpp")

add_library(desyl_lib desyllib.cpp ${HEADER_LIST})

target_include_directories(desyl_lib PUBLIC ../include)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${HEADER_LIST}
)
