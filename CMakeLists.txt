cmake_minimum_required(VERSION 3.23)
project("simple-thread-pool")

add_executable(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23)
target_sources(
    ${PROJECT_NAME}
    PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
    FILES
        src/main.cppm
        src/lib.cppm
        src/ThreadPool.cppm
)

add_library(std_module)
set_target_properties(std_module PROPERTIES CXX_STANDARD 23)
target_sources(
    std_module
    PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
    FILES
        std_module/std.ixx
        std_module/std.compat.ixx
)

target_link_libraries(${PROJECT_NAME} PRIVATE std_module)