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