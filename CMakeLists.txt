cmake_minimum_required(VERSION 3.30)

project(Caracal)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

include(GNUInstallDirs)
if(WIN32)
    if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endif()
endif()

include(CaraTest)

add_subdirectory(Source)
add_subdirectory(Driver)

add_subdirectory(Tests/AllInOne)
add_subdirectory(Tests/CppCodeGenTests)
add_subdirectory(Tests/LexerTests)
add_subdirectory(Tests/ParserTests)
add_subdirectory(Tests/SourceLocationTests)
add_subdirectory(Tests/TypeCheckerTests)
add_subdirectory(Tests/VirtualMachineTests)
