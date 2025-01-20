
set( app_name demo_01 )

set( sources
    "main.cpp"
    "app_camera.h"
    )

add_executable( ${app_name} ${sources} ) 
target_link_libraries( ${app_name} 
      PUBLIC motor::io 
      PUBLIC motor::concurrent 
      PUBLIC motor::memory
      PUBLIC motor::application
      PUBLIC motor::graphics
      PUBLIC motor::platform
      PUBLIC motor::gfx
      PUBLIC motor::property
      PUBLIC motor::format
      )

    set_target_properties( ${app_name} PROPERTIES FOLDER "demos/demo_01" )

    set( data_path ${CMAKE_CURRENT_LIST_DIR} ) 
    target_compile_definitions( ${app_name} PUBLIC -DDATAPATH="${data_path}")

motor_vs_src_dir( ${sources} ) 

install( TARGETS ${app_name} EXPORT suites-targets 
   RUNTIME DESTINATION bin )


