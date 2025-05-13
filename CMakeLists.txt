cmake_minimum_required(VERSION 3.20)

project(Aalenian)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core)
qt_standard_project_setup()

include(CMake/AalTest.cmake)

add_subdirectory(Compiler)
add_subdirectory(Driver)

add_subdirectory(Tests/AllInOne)
add_subdirectory(Tests/LexerTests)
add_subdirectory(Tests/ParserTests)
add_subdirectory(Tests/TypeCheckerTests)
add_subdirectory(Tests/VirtualMachineTests)
add_subdirectory(Tests/SourceLocationTests)
