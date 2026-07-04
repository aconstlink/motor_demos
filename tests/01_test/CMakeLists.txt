
set( app_name 01_scene_test )

set( commons 
  "../../common/audio_analysis.h"
    "../../common/audio_analysis.cpp"
    "../../common/scene/iscene.h"
    "../../common/scene/dummy_scene.hpp"
    "../../common/scene/stalled_scene.hpp"
    "../../common/scene/simple_gltf_scene.hpp"
    "../../common/camera_manager.h"
    "../../common/camera_manager.cpp"
    "../../common/scene/scene_manager.h"
    "../../common/scene/scene_manager.cpp"
    "../../common/the_app/the_app.h"
    "../../common/the_app/the_app.cpp"
    "../../common/the_app/on_audio.cpp"
    "../../common/the_app/on_tool.cpp"
    "../../common/camera_collector.hpp"

    "../../common/types.hpp"
)

set( sources
    "main.h"
    "main.cpp"
    
    ${commons}
    
    )

motor_vs_src_dir( sources )

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

    set_target_properties( ${app_name} PROPERTIES FOLDER "tests" )

    set( data_path ${CMAKE_CURRENT_LIST_DIR} ) 
    target_compile_definitions( ${app_name} PUBLIC -DDATAPATH="${data_path}")

motor_vs_src_dir( ${sources} ) 

install( TARGETS ${app_name} EXPORT suites-targets 
   RUNTIME DESTINATION bin )


