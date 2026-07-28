add_custom_target(copy_core_files_buildstep ALL
    COMMAND ${CMAKE_COMMAND}
        -DCARA_SOURCE_DIRECTORY="${CARA_ROOT_DIR}/Core"
        -DCARA_DESTINATION_DIRECTORY="${CMAKE_BINARY_DIR}/Core"
        -P "${CARA_ROOT_DIR}/CMake/SyncDirectory.cmake"
    COMMAND ${CMAKE_COMMAND}
        -DCARA_SOURCE_DIRECTORY="${CARA_ROOT_DIR}/Prelude"
        -DCARA_DESTINATION_DIRECTORY="${CMAKE_BINARY_DIR}/Prelude"
        -P "${CARA_ROOT_DIR}/CMake/SyncDirectory.cmake"
    COMMENT "Synchronizing Core and Prelude files to build directory"
)
