add_subdirectory(automata)
add_subdirectory(pugixml)
add_subdirectory(app)

install(TARGETS
    automata
    pugixml
    app
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

install(
    FILES
        ${CMAKE_BINRAY_DIR}/automata_export.hpp
        ${CMAKE_SOURCE_DIR}/automata.hpp
    DESTINATION 
        include/automata
)
install(
    FILES
        ${CMAKE_BINRAY_DIR}/pugixml_export.hpp
        ${CMAKE_SOURCE_DIR}/pugixml.hpp
    DESTINATION 
        include/pugixml
)