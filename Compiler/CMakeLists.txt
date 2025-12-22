project(Compiler)

file(GLOB SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/source/**/*.cpp")
file(GLOB HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/source/**/*.h")

qt_add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/source")
#target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}
             BASE_NAME ${PROJECT_NAME}
             EXPORT_MACRO_NAME COMPILER_API
             EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/source/${PROJECT_NAME}/API.h
)
