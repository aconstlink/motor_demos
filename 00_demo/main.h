
#include "app_camera.h"

#include <motor/platform/global.h>

#include <motor/tool/imgui/timeline.h>
#include <motor/tool/imgui/player_controller.h>

#include <motor/controls/types/ascii_keyboard.hpp>
#include <motor/controls/types/three_mouse.hpp>

#include <motor/graphics/object/framebuffer_object.h>

#include <motor/gfx/primitive/primitive_render_3d.h>
#include <motor/gfx/camera/generic_camera.h>

#include <motor/math/utility/fn.hpp>
#include <motor/math/utility/angle.hpp>
#include <motor/math/animation/keyframe_sequence.hpp>
#include <motor/math/quaternion/quaternion4.hpp>
#include <motor/math/utility/time.hpp>

#include <motor/log/global.h>
#include <motor/memory/global.h>
#include <motor/concurrent/global.h>

#include <future>


namespace demos
{
    using namespace motor::core::types ;

    class the_app : public motor::application::app
    {
        motor_this_typedefs( the_app ) ;

        motor::graphics::state_object_t rs ;

        // debug renderer
        motor::gfx::primitive_render_3d_t pr ;

    private: // camera 

        // 0 : free camera
        // 1 : spline camera
        motor::gfx::generic_camera_t camera[ 2 ] ;
        size_t cam_idx = 0 ;

        helper::camera_controls::control_vector _cc ;


    private: // tool ui

        size_t _twid = size_t( -1 ) ;
        motor::tool::player_controller_t pc ;
        motor::tool::timeline_t tl = motor::tool::timeline_t( "my timeline" ) ;

    private: // time control

        size_t _max_time_milli = motor::math::time::to_milli( 3, 11, 10 ) ;
        bool_t _proceed_time = false ;
        size_t cur_time = 0 ;

    private: // render window

        size_t _rwid = size_t( -1 ) ;
        motor::math::vec4ui_t fb_dims = motor::math::vec4ui_t( 0, 0, 1920, 1080 ) ;
        motor::graphics::framebuffer_object_t pp_fb ;

    private: // debug window
        
        size_t _dwid = size_t( -1 ) ;

    private:

        virtual void_t on_init( void_t ) noexcept ;
        virtual void_t on_event( window_id_t const wid,
            motor::application::window_message_listener::state_vector_cref_t sv ) noexcept ;
        virtual void_t on_device( device_data_in_t dd ) noexcept ;
        virtual void_t on_update( motor::application::app::update_data_in_t ) noexcept ;
        virtual void_t on_graphics( motor::application::app::graphics_data_in_t gd ) noexcept ;
        virtual void_t on_render( this_t::window_id_t const wid, motor::graphics::gen4::frontend_ptr_t fe,
            motor::application::app::render_data_in_t rd ) noexcept ;
        virtual bool_t on_tool( this_t::window_id_t const wid, motor::application::app::tool_data_ref_t td ) noexcept ;
    } ;
}