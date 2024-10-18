add_subdirectory(fmt-master)
add_subdirectory(scope_checker)
add_subdirectory(parser)
add_subdirectory(lexer)
add_subdirectory(automata)
add_subdirectory(pugixml)
add_subdirectory(app)
add_subdirectory(testing)
add_subdirectory(ast_node)

install(TARGETS
    fmt
    scope_checker
    parser
    lexer
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
        ${CMAKE_BINRAY_DIR}/lexer_export.hpp
        ${CMAKE_SOURCE_DIR}/lexer.hpp
    DESTINATION 
        include/lexer
)

install(
    FILES
        ${CMAKE_BINRAY_DIR}/scope_checker_export.hpp
        ${CMAKE_SOURCE_DIR}/scope_checker.hpp
    DESTINATION 
        include/scope_checker
)

install(
    FILES
        ${CMAKE_BINRAY_DIR}/parser_export.hpp
        ${CMAKE_SOURCE_DIR}/parser.hpp
    DESTINATION 
        include/parser
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
install(
    FILES
        ${CMAKE_BINRAY_DIR}/testing_export.hpp
        ${CMAKE_SOURCE_DIR}/testing.hpp
    DESTINATION 
        include/testing
)
install(
    FILES
        ${CMAKE_BINRAY_DIR}/ast_node_export.hpp
        ${CMAKE_SOURCE_DIR}/ast_node.hpp
    DESTINATION 
        include/ast_node
)