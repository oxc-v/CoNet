project(test)

add_subdirectory(src)

add_executable(test main.cpp)
target_link_libraries(test conet)