add_subdirectory(pugixml)
add_subdirectory(app)

install(TARGETS
    pugixml
    app
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

install(
    FILES
        ${CMAKE_SOURCE_DIR}/pugixml.hpp
        ${CMAKE_BINRAY_DIR}/pugixml_export.hpp
    DESTINATION 
        include/pugixml
)