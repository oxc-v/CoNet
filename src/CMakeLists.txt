FILE(GLOB SOURCE_FILE "*.cpp")
FILE(GLOB INCLUDE_FILE "*.hpp")

ADD_LIBRARY(conet STATIC ${SOURCE_FILE} ${INCLUDE_FILE})

target_include_directories(conet INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(conet PUBLIC -fcoroutines)
target_compile_features(conet PUBLIC cxx_std_20)