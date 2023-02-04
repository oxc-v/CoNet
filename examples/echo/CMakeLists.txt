FILE(GLOB SOURCE_FILE "*.cpp")
FILE(GLOB INCLUDE_FILE "*.hpp")

add_executable(echo ${SOURCE_FILE} ${INCLUDE_FILE})

target_link_libraries(echo PUBLIC conet)
target_compile_features(echo PUBLIC cxx_std_20)