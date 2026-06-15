
#pragma once

#include "../scene/scene_manager.h"
#include "../audio_analysis.h"
#include "../types.hpp"

#include <motor/platform/global.h>
#include <motor/gfx/primitive/primitive_render_3d.h>

#include <motor/tool/imgui/timeline.h>
#include <motor/tool/imgui/player_controller.h>

#include <motor/io/monitor.hpp>
#include <motor/io/database.h>
#include <motor/io/global.h>

#include <motor/std/vector>
#include <motor/log/global.h>
#include <motor/memory/global.h>
#include <motor/concurrent/global.h>

#include <motor/math/utility/time.hpp>

namespace demos
{

//******************************************************************
class the_app : public motor::application::app
{
    motor_this_typedefs( the_app );

    // debug renderer
    motor::gfx::primitive_render_3d_t pr;
    motor::graphics::state_object_t _pr_rs;

    // debug view root render states
    motor::graphics::state_object_t _dv_rs;

    motor::audio::capture_object_t _co;
    demos::audio_analysis _aanl;

#if 0 // PRODUCTION_MODE
        demos::demo_mode const _dm = demos::demo_mode::production ;
#else
    demos::demo_mode const _dm = demos::demo_mode::tool;
#endif

    bool_t is_tool_mode( void_t ) const noexcept
    {
        return _dm == demos::demo_mode::tool;
    }

  private: // file

    motor::io::monitor_mtr_t _mon = nullptr;
    motor::io::database_mtr_t _db = nullptr;

  private: // scenes

    demos::scene_manager_mtr_t _sm = nullptr;

  private: // free camera

#if 0
    bool_t _use_free_camera = true;
    motor::gfx::generic_camera_t _camera;
    // this is only for user input interaction!
    helper::camera_controls::control_vector _cc;
#endif

  private: // tool ui

    size_t _twid = size_t( -1 );
    motor::tool::player_controller_t pc;
    motor::tool::timeline_t tl = motor::tool::timeline_t( "my timeline" );
    bool_t _space_bar_pressed = false;

    // could be used in production
    bool_t _need_tool_view = false;

#if 1
    size_t _jump_to_scene = 0;
#else
    size_t _jump_to_scene = size_t( -1 );
#endif

  private: // time control

    size_t _max_time_milli = motor::math::time::to_milli( 3, 11, 10 );
    bool_t _proceed_time = false;
    size_t _cur_time = 0;

  private: // final render stuff

    bool_t _gbuffer_sel_changed = false;
    size_t _gbuffer_selection = 2;
    motor::graphics::state_object_t _scene_final_rs;

  private: // post process

    size_t _rwid = size_t( -1 );
    motor::math::vec4ui_t fb_dims = motor::math::vec4ui_t( 0, 0, 1920, 1080 );
    motor::graphics::framebuffer_object_t pp_fb0; // the 1st scene is rendered to
    motor::graphics::framebuffer_object_t pp_fb1; // the 2nd scene is rendered to

    motor::graphics::msl_object_mtr_t _post_msl = nullptr;
    motor::graphics::msl_object_mtr_t _post_xfade_msl = nullptr;
    motor::graphics::geometry_object_mtr_t _post_quad = nullptr;
    motor::graphics::state_object_t _post_process_rs;

  private: // debug window

    size_t _dwid = size_t( -1 );

  private:

    virtual void_t on_init( void_t ) noexcept;
    virtual void_t
    on_event( window_id_t const wid,
              motor::application::window_message_listener::state_vector_cref_t sv ) noexcept;
    virtual void_t on_device( device_data_in_t dd ) noexcept;
    virtual void_t on_update( motor::application::app::update_data_in_t ) noexcept;
    virtual void_t the_app::on_audio( motor::audio::frontend_ptr_t fptr,
                                      audio_data_in_t ad ) noexcept;
    virtual void_t on_graphics( motor::application::app::graphics_data_in_t gd ) noexcept;
    virtual void_t on_render( this_t::window_id_t const wid,
                              motor::graphics::gen4::frontend_ptr_t fe,
                              motor::application::app::render_data_in_t rd ) noexcept;
    virtual bool_t on_tool( this_t::window_id_t const wid,
                            motor::application::app::tool_data_ref_t td ) noexcept;
    virtual void_t on_shutdown( void_t ) noexcept;

  public:

    the_app( demos::scene_manager_mtr_safe_t ) noexcept;
    the_app( this_rref_t ) noexcept;
    virtual ~the_app( void_t ) noexcept;
};

} // namespace demos