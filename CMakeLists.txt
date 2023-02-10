project(test)

add_subdirectory(src)
add_subdirectory(examples/echo)
add_subdirectory(examples/http)

add_executable(test main.cpp)
target_link_libraries(test conet)