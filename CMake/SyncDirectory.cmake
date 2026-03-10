# synchronizes source directory with destination directory
# removes old files from destination that don't exist in source 
# and copies files that are new or changed.

message(STATUS "SyncDirectory.cmake started")

if(NOT DEFINED CARA_SOURCE_DIRECTORY)
    message(FATAL_ERROR "CARA_SOURCE_DIRECTORY must be defined with -DCARA_SOURCE_DIRECTORY=path/to/source")
endif()

if(NOT DEFINED CARA_DESTINATION_DIRECTORY)
    message(FATAL_ERROR "CARA_DESTINATION_DIRECTORY must be defined via -DCARA_DESTINATION_DIRECTORY=path/to/destination")
endif()

message(STATUS "Syncing directory: ${CARA_SOURCE_DIRECTORY} -> ${CARA_DESTINATION_DIRECTORY}")

file(GLOB_RECURSE CARA_SOURCE_FILES RELATIVE "${CARA_SOURCE_DIRECTORY}" "${CARA_SOURCE_DIRECTORY}/*.cara")
file(GLOB_RECURSE CARA_DESTINATION_FILES RELATIVE "${CARA_DESTINATION_DIRECTORY}" "${CARA_DESTINATION_DIRECTORY}/*.cara")

# Remove old files
foreach(CARA_DESTINATION_FILE IN LISTS CARA_DESTINATION_FILES)
    list(FIND CARA_SOURCE_FILES "${CARA_DESTINATION_FILE}" CARA_FOUND_INDEX)
    if(CARA_FOUND_INDEX EQUAL -1)
        message(STATUS "Removing outdated file: ${CARA_DESTINATION_DIRECTORY}/${CARA_DESTINATION_FILE}")
        file(REMOVE "${CARA_DESTINATION_DIRECTORY}/${CARA_DESTINATION_FILE}")
    endif()
endforeach()

# Copy changed files
foreach(CARA_SOURCE_FILE IN LISTS CARA_SOURCE_FILES)
    set(CARA_SOURCE_FILE_PATH "${CARA_SOURCE_DIRECTORY}/${CARA_SOURCE_FILE}")
    set(CARA_DESTINATION_FILE_PATH "${CARA_DESTINATION_DIRECTORY}/${CARA_SOURCE_FILE}")
    get_filename_component(CARA_DESTINATION_PARENT_DIR "${CARA_DESTINATION_FILE_PATH}" DIRECTORY)
    file(MAKE_DIRECTORY "${CARA_DESTINATION_PARENT_DIR}")
    execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CARA_SOURCE_FILE_PATH}" "${CARA_DESTINATION_FILE_PATH}")
endforeach()

message(STATUS "SyncDirectory.cmake finished for ${CARA_SOURCE_DIRECTORY}")
