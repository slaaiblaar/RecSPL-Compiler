set(TARGET_NAME app)
set(TARGET_INCLUDE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(TARGET_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable(${TARGET_NAME} ${TARGET_SOURCES})

target_include_directories(${TARGET_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/exports>
  PRIVATE 
    ${TARGET_INCLUDE_FOLDER}
    ${CMAKE_CURRENT_SOURCE_DIR}/../pugixml/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../automata/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../lexer/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../parser/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../scope_checker/include
    # Any other extra include required
)

target_link_libraries(${TARGET_NAME} PUBLIC pugixml)
target_link_libraries(${TARGET_NAME} PUBLIC automata)
target_link_libraries(${TARGET_NAME} PUBLIC lexer)
target_link_libraries(${TARGET_NAME} PUBLIC parser)
target_link_libraries(${TARGET_NAME} PUBLIC scope_checker)