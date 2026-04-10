cmake_minimum_required(VERSION 4.0)
project(CppCourseDesign)

set(CMAKE_CXX_STANDARD 11)

add_executable(CppCourseDesign main.cpp)

target_link_libraries(CppCourseDesign ws2_32)
target_compile_definitions(CppCourseDesign PRIVATE _WIN32_WINNT=0x0A00)
