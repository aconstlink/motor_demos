
#pragma once

#include "../types.hpp"
#include "../camera_manager.h"

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
    iscene( this_rref_t rhv ) noexcept : _name( std::move( rhv._name ) ), _ti( rhv._ti)
    {
    }

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

    virtual void_t on_init( motor::io::database_ptr_t ) noexcept = 0;
    virtual void_t on_release( void_t ) noexcept = 0;

    virtual void_t on_resize(
        demos::window_type const, uint_t const width, uint_t const height ) noexcept = 0;

    virtual void_t on_graphics( demos::iscene::on_graphics_data_in_t ) noexcept = 0;

    virtual void_t on_render_init( demos::window_type const, motor::graphics::gen4::frontend_ptr_t,
        motor::graphics::gen4::frontend::fence_funk_t ) noexcept = 0;
    virtual void_t on_render_deinit( demos::window_type const,
        motor::graphics::gen4::frontend_ptr_t,
        motor::graphics::gen4::frontend::fence_funk_t ) noexcept = 0;

    virtual void_t on_render_debug(
        size_t const wid, motor::graphics::gen4::frontend_ptr_t ) noexcept = 0;
    virtual void_t on_render_final(
        size_t const wid, motor::graphics::gen4::frontend_ptr_t ) noexcept = 0;

    virtual void_t on_tool( void_t ) noexcept = 0;

  public: // update interface

    struct update_data
    {
        motor::math::time_ms_t absolute;
        motor::math::time_ms_t relative;

        float_t relative_seconds;
    };
    motor_typedef( update_data );

    virtual void_t on_update( demos::iscene::update_data_cref_t ) noexcept = 0;
};
motor_typedef( iscene );
} // namespace demos