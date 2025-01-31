
#include "app_camera.h"

#include <motor/platform/global.h>

#include <motor/tool/imgui/timeline.h>
#include <motor/tool/imgui/player_controller.h>

#include <motor/controls/types/ascii_keyboard.hpp>
#include <motor/controls/types/three_mouse.hpp>

#include <motor/graphics/object/framebuffer_object.h>

#include <motor/gfx/primitive/primitive_render_3d.h>
#include <motor/gfx/camera/generic_camera.h>

#include "../common/audio_analysis.h"
#include "../common/iscene.h"

#include <motor/math/utility/fn.hpp>
#include <motor/math/utility/angle.hpp>

#include <motor/math/quaternion/quaternion4.hpp>
#include <motor/math/utility/time.hpp>

#include <motor/std/vector>
#include <motor/log/global.h>
#include <motor/memory/global.h>
#include <motor/concurrent/global.h>

#include <future>


namespace demos
{
    using namespace motor::core::types ;

    //******************************************************************
    class the_app : public motor::application::app
    {
        motor_this_typedefs( the_app ) ;

        // debug renderer
        motor::gfx::primitive_render_3d_t pr ;
        motor::graphics::state_object_t _pr_rs ;

        // debug view root render states
        motor::graphics::state_object_t _dv_rs ;

        motor::audio::capture_object_t _co ;
        demos::audio_analysis _aanl ;

    private: // file

        motor::io::monitor_mtr_t _mon = motor::memory::create_ptr( motor::io::monitor_t() ) ;
        motor::io::database _db = motor::io::database( motor::io::path_t( DATAPATH ), "./working", "data" ) ;


    private: // scenes

        motor::vector< demos::iscene_mtr_t > _scenes ;
        demos::iscene_mtr_t get_current_scene( void_t ) noexcept ;

    private: // free camera 
      

        bool_t _use_free_camera = true ;
        motor::gfx::generic_camera_t _camera ;
        // this is only for user input interaction!
        helper::camera_controls::control_vector _cc ;

    private: // tool ui

        size_t _twid = size_t( -1 ) ;
        motor::tool::player_controller_t pc ;
        motor::tool::timeline_t tl = motor::tool::timeline_t( "my timeline" ) ;

    private: // time control

        size_t _max_time_milli = motor::math::time::to_milli( 3, 11, 10 ) ;
        bool_t _proceed_time = false ;
        size_t cur_time = 0 ;

    private: // final render stuff

        bool_t _gbuffer_sel_changed = false ;
        size_t _gbuffer_selection = 2 ;
        motor::graphics::state_object_t _scene_final_rs ;

    private: // post process

        size_t _rwid = size_t( -1 ) ;
        motor::math::vec4ui_t fb_dims = motor::math::vec4ui_t( 0, 0, 1920, 1080 ) ;
        motor::graphics::framebuffer_object_t pp_fb ; // the scene is rendered to
        
        motor::graphics::msl_object_mtr_t _post_msl ;
        motor::graphics::geometry_object_mtr_t _post_quad ;
        motor::graphics::state_object_t _post_process_rs ;

    private: // debug window
        
        size_t _dwid = size_t( -1 ) ;

    private:

        virtual void_t on_init( void_t ) noexcept ;
        virtual void_t on_event( window_id_t const wid,
            motor::application::window_message_listener::state_vector_cref_t sv ) noexcept ;
        virtual void_t on_device( device_data_in_t dd ) noexcept ;
        virtual void_t on_update( motor::application::app::update_data_in_t ) noexcept ;
        virtual void_t the_app::on_audio( motor::audio::frontend_ptr_t fptr, audio_data_in_t ad ) noexcept ;
        virtual void_t on_graphics( motor::application::app::graphics_data_in_t gd ) noexcept ;
        virtual void_t on_render( this_t::window_id_t const wid, motor::graphics::gen4::frontend_ptr_t fe,
            motor::application::app::render_data_in_t rd ) noexcept ;
        virtual bool_t on_tool( this_t::window_id_t const wid, motor::application::app::tool_data_ref_t td ) noexcept ;
        virtual void_t on_shutdown( void_t ) noexcept ;

        motor::string_t make_camera_data_file( void_t ) noexcept ;

    public:

        the_app( void_t ) noexcept ;
        the_app( this_rref_t ) noexcept ;
        virtual ~the_app( void_t ) noexcept ;
    } ;
}