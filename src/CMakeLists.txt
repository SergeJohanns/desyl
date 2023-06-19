set(HEADER_LIST "${PROJECT_SOURCE_DIR}/include/desyl/desyllib.hpp")

add_library(desyl_lib desyllib.cpp)
add_subdirectory(ast)
add_subdirectory(parser)
add_subdirectory(context)
add_subdirectory(synthesis)
target_link_libraries(desyl_lib PRIVATE ast parser context synthesis)

target_include_directories(desyl_lib PUBLIC ../include)
target_include_directories(desyl_lib PRIVATE . PRIVATE ./include)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${HEADER_LIST}
)
