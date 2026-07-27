include(FetchContent)

set(CARAREPORT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CARAREPORT_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)

FetchContent_Declare(CaraReport
    GIT_REPOSITORY https://github.com/Caracal-Lang/CaraReport.git
    GIT_TAG 55d04fe811bd944e0122e9c78cde66bd0c033e2b)

FetchContent_MakeAvailable(CaraReport)

if(NOT WIN32)
    # Make the library position independent for use in shared libraries
    set_target_properties(CaraReport PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
