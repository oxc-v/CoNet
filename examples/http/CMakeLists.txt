FILE(GLOB SOURCE_FILE "*.cpp")
FILE(GLOB INCLUDE_FILE "*.hpp")

add_executable(http ${SOURCE_FILE} ${INCLUDE_FILE})

target_link_libraries(http PUBLIC conet)
target_compile_features(http PUBLIC cxx_std_20)