
#pragma once

#include "../types.hpp"

#include <motor/gfx/primitive/primitive_render_3d.h>
#include <motor/gfx/camera/generic_camera.h>
#include <motor/gfx/manager/msl_manager.h>

#include <motor/graphics/frontend/gen4/frontend.hpp>
#include <motor/io/database.h>
#include <motor/concurrent/task/task.hpp>

namespace demos
{
using namespace motor::core::types;

//**********************************************************
// Base scene
class iscene
{
    motor_this_typedefs( iscene );

  public:

    typedef struct
    {
        motor::math::time_ms_t duration;
        motor::math::time_ms_t set_back;
    } timing_info_t;

    struct on_graphics_data
    {
        size_t const cur_time;
        float_t const dt;
        motor::gfx::primitive_render_3d_ptr_t pr;
        motor::gfx::generic_camera_mtr_t dbg_cam;
    };
    motor_typedef( on_graphics_data );

    struct on_init_data
    {
        motor::io::database_ptr_t db;
        motor::gfx::msl_manager_mtr_t mmgr ;
    };
    motor_typedef( on_init_data );

    struct on_release_data
    {
    };
    motor_typedef( on_release_data );

    struct on_frame_done_data
    {
    };
    motor_typedef( on_frame_done_data );

    struct on_render_data
    {
        size_t wid;
        demos::window_type wt;
        motor::graphics::gen4::frontend_ptr_t fe;
    };
    motor_typedef( on_render_data );

    struct on_tool_data
    {
    };
    motor_typedef( on_tool_data );

    struct update_data
    {
        motor::math::time_ms_t absolute;
        motor::math::time_ms_t relative;

        float_t relative_seconds;
    };
    motor_typedef( update_data );

  private:

    motor::string_t _name;
    timing_info_t _ti;

  public:

    iscene( motor::string_in_t name, motor::math::time_ms_t const dur ) noexcept
        : _name( name ), _ti( { dur, 2000 } )
    {
    }

    iscene( motor::string_in_t name, this_t::timing_info_t const & ti ) noexcept
        : _name( name ), _ti( ti )
    {
    }

    iscene( this_cref_t ) = delete;
    iscene( this_rref_t rhv ) noexcept : _name( std::move( rhv._name ) ), _ti( rhv._ti ) {}

    virtual ~iscene( void_t ) noexcept {}

  public: //

    motor::string_cref_t name( void_t ) const noexcept
    {
        return _name;
    }

  public: // interface

    virtual timing_info_t get_timing_info( void_t ) const noexcept
    {
        return _ti;
    }

    virtual void_t on_init( demos::iscene::on_init_data_in_t d ) noexcept = 0;
    virtual void_t on_release( demos::iscene::on_release_data_in_t ) noexcept = 0;

    virtual void_t on_resize(
        demos::window_type const, uint_t const width, uint_t const height ) noexcept = 0;

    virtual void_t on_graphics( demos::iscene::on_graphics_data_in_t ) noexcept = 0;
    virtual void_t on_frame_done( demos::iscene::on_frame_done_data_in_t ) noexcept = 0;

    virtual void_t on_render_init( demos::iscene::on_render_data_in_t,
        motor::graphics::gen4::frontend::fence_funk_t ) noexcept = 0;
    virtual void_t on_render_deinit( demos::iscene::on_render_data_in_t,
        motor::graphics::gen4::frontend::fence_funk_t fence ) noexcept = 0;

    virtual void_t on_render_debug( demos::iscene::on_render_data_in_t ) noexcept = 0;

    virtual void_t on_render_final_offscreen( demos::iscene::on_render_data_in_t ) noexcept = 0;

    virtual void_t on_render_final_depth_pass( demos::iscene::on_render_data_in_t ) noexcept = 0;
    virtual void_t on_render_final( demos::iscene::on_render_data_in_t ) noexcept = 0;

    virtual void_t on_tool( demos::iscene::on_tool_data_in_t ) noexcept = 0;

  public: // update interface

    

    virtual void_t on_update( demos::iscene::update_data_cref_t ) noexcept = 0;
};
motor_typedef( iscene );
} // namespace demos