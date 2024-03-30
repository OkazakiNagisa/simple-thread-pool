cmake_minimum_required(VERSION 3.23)
project("simple-thread-pool")

# library target
add_library(${PROJECT_NAME} INTERFACE)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23)
target_include_directories(${PROJECT_NAME} INTERFACE src/include)

# library test demo target
add_executable(${PROJECT_NAME}_demo
    example/main.cc
)
set_target_properties(${PROJECT_NAME}_demo PROPERTIES CXX_STANDARD 23)
target_link_libraries(${PROJECT_NAME}_demo PRIVATE ${PROJECT_NAME})