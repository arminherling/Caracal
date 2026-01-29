project(AllInOne)

add_executable(
    ${PROJECT_NAME} 
    "${PROJECT_SOURCE_DIR}/../CppCodeGenTests/Source/CppCodeGenTests.cpp"
    "${PROJECT_SOURCE_DIR}/../LLVMCodeGenTests/Source/LLVMCodeGenTests.cpp"
    "${PROJECT_SOURCE_DIR}/../LexerTests/Source/LexerTests.cpp"
    "${PROJECT_SOURCE_DIR}/../ParserTests/Source/ParserTests.cpp"
    "${PROJECT_SOURCE_DIR}/../TypeCheckerTests/Source/TypeCheckerTests.cpp"
    "${PROJECT_SOURCE_DIR}/../SourceLocationTests/Source/SourceLocationTests.cpp"
    "${PROJECT_SOURCE_DIR}/../VirtualMachineTests/Source/VirtualMachineTests.cpp"
    "Source/main.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE Caracal CaraTest)

if (WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
    )
endif ()
