add_executable(desyl desyl.cpp)

target_link_libraries(desyl PRIVATE desyl_lib gflags::gflags)
