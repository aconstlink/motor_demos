
set( app_name 00_demo )

set( sources
    "../common/audio_analysis.h"
    "../common/audio_analysis.cpp"
    "../common/iscene.h"
    "../common/camera_manager.h"
    "../common/camera_manager.cpp"

    "main.h"
    "main.cpp"
    "app_camera.h"
    "on_graphics.cpp"
    "on_render.cpp"
    "on_tool.cpp"
    "on_audio.cpp"

    
    "scenes/scene_0.h"
    "scenes/scene_0.cpp"
    "scenes/scene_0_coreos.cpp"
    "scenes/scene_1.h"
    "scenes/scene_1.cpp"
    "scenes/scene_1_coreos.cpp"

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


