option(CARACAL_TRACY "Enable Tracy profiling" OFF)
set(CARACAL_TRACY_TAG "v0.13.1" CACHE STRING "The pinned Tracy release Caracal profiles against")

if(NOT CARACAL_TRACY)
    return()
endif()

include(FetchContent)
message(STATUS "Caracal profiling ON")

# TRACY_ENABLE is Tracy's own switch: without it the client compiles to a stub, which would leave us with
# the dependency and none of the data. TRACY_ON_DEMAND means the compiler runs at full speed until a
# profiler actually connects, so an instrumented build is still usable as a build.
set(TRACY_ENABLE ON CACHE BOOL "" FORCE)
set(TRACY_ON_DEMAND ON CACHE BOOL "" FORCE)
set(TRACY_NO_BROADCAST ON CACHE BOOL "" FORCE)

FetchContent_Declare(tracy
    GIT_REPOSITORY https://github.com/wolfpld/tracy.git
    GIT_TAG        ${CARACAL_TRACY_TAG}
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE)
FetchContent_MakeAvailable(tracy)

add_compile_definitions(CARACAL_TRACY)
