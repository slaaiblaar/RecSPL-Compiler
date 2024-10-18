set(TARGET_NAME scope_checker)
set(TARGET_INCLUDE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(TARGET_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/scope_checker.cpp)


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
    ${CMAKE_CURRENT_SOURCE_DIR}/../pugixml/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../automata/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../fmt/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../fmt-master/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../ast_node/include
)
target_link_libraries(${TARGET_NAME} PUBLIC pugixml)
target_link_libraries(${TARGET_NAME} PUBLIC automata)
target_link_libraries(${TARGET_NAME} PUBLIC fmt)
target_link_libraries(${TARGET_NAME} PUBLIC ast_node)