project(test)

add_subdirectory(src)
add_subdirectory(examples/echo)

add_executable(test main.cpp)
target_link_libraries(test conet)