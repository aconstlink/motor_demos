cmake_minimum_required( VERSION 3.20 )
project( "motor_demos" )

# required by motor
set( CMAKE_CXX_STANDARD 17 )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build )

add_subdirectory( motor )
list( APPEND CMAKE_MODULE_PATH 
  "${CMAKE_CURRENT_LIST_DIR}/motor/cmake/macros" )

include( visual_studio_generate_bat_dll )
include( emacs_default_directory )

motor_emacs_default_directory( "${CMAKE_CURRENT_LIST_DIR}/motor" )

set( CMAKE_DEBUG_POSTFIX d )

option( DEMO_PRODUCTION_MODE "Demo Mode" OFF )

if( ${DEMO_PRODUCTION_MODE} )
add_definitions( -DPRODUCTION_MODE )
else()
add_definitions( -DDEBUG_MODE )
endif()

add_subdirectory( "00_demo" )



install( EXPORT suites-targets DESTINATION "cmake" )

