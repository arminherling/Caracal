include(FetchContent)

set(CARAREPORT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CARAREPORT_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)

FetchContent_Declare(CaraReport
    GIT_REPOSITORY https://github.com/Caracal-Lang/CaraReport.git
    GIT_TAG c4933160c6aa98e7de52893aa2499d2bd4857b4b)

FetchContent_MakeAvailable(CaraReport)
