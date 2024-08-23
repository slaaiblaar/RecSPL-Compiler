set(TARGET_NAME automata)
set(TARGET_INCLUDE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(TARGET_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/automata.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/src/state.cpp
                   ${CMAKE_CURRENT_SOURCE_DIR}/src/tree.cpp)


if(BUILD_SHARED_LIBS)
  add_library(${TARGET_NAME} SHARED ${TARGET_SOURCES})
else()
  add_library(${TARGET_NAME} STATIC ${TARGET_SOURCES})
endif()

generate_export_header(${TARGET_NAME} EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/${TARGET_NAME}_export.h)

set_target_properties(${TARGET_NAME} PROPERTIES 
                                     DEBUG_POSTFIX "-d" 
                                     MSVC_RUNTIME_LIBRARY "${MSVC_RUNTIME_TYPE}" 
                                     VERSION ${CMAKE_PROJECT_VERSION}
                                     SOVERSION ${CMAKE_PROJECT_VERSION})

target_compile_options(${TARGET_NAME} PRIVATE ${compiler_options})

target_link_options(${TARGET_NAME} PRIVATE ${linker_flags})

target_include_directories(${TARGET_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/exports>
  PRIVATE 
    ${TARGET_INCLUDE_FOLDER}
)