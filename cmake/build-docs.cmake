find_package( Doxygen )
if( DOXYGEN_FOUND )

  file(GLOB CANDIDATE_DOC_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/*)

  set(DOXYGEN_INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src )
  set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs )
  set(DOXYGEN_README ${CMAKE_CURRENT_SOURCE_DIR}/cmake/docroot.md)
  set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Doxyfile.in )
  set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/Doxyfile)
  configure_file( ${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY )
  file( MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR} )
  add_custom_command(
    OUTPUT ${DOXYGEN_OUTPUT_DIR}/html/index.html
    DEPENDS ${CANDIDATE_DOC_SOURCE}
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
    MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
    COMMENT "Generating Documents" )
  
  add_custom_target( Doxygen ALL DEPENDS ${DOXYGEN_OUTPUT_DIR}/html/index.html )
    
else()
  message( WARNING "Doxygen is not installed in this system. Unable to generate documentation" )
endif()
