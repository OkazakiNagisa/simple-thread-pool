cmake_minimum_required(VERSION 3.28)
project("simple-thread-pool")

find_package(Catch2 CONFIG REQUIRED)

# simple thread pool library target
add_library(${PROJECT_NAME} INTERFACE)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23)
target_include_directories(${PROJECT_NAME} INTERFACE src/include)

add_library(${PROJECT_NAME}_cxxmodule)
set_target_properties(${PROJECT_NAME}_cxxmodule PROPERTIES CXX_STANDARD 23)
target_sources(
    ${PROJECT_NAME}_cxxmodule
    PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
    FILES
        src/cxx20_module/stp.cppm
        src/cxx20_module/stp/ThreadPool.cppm
)

# demo target
add_executable(stp_demo
    example/main.cc
)
set_target_properties(stp_demo PROPERTIES CXX_STANDARD 23)
target_link_libraries(stp_demo PRIVATE ${PROJECT_NAME})

# add_library(std_cxxmodule)
# set_target_properties(std_cxxmodule PROPERTIES CXX_STANDARD 23)
# target_sources(
#     std_cxxmodule
#     PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES
#     FILES
#         example/cxx20_module/std/std.compat.ixx
#         example/cxx20_module/std/std.ixx
# )

add_executable(stp_demo_cxxmodule
    example/cxx20_module/main.cc
)
set_target_properties(stp_demo_cxxmodule PROPERTIES CXX_STANDARD 23)
target_link_libraries(stp_demo_cxxmodule PRIVATE ${PROJECT_NAME}_cxxmodule)