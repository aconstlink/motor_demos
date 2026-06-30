#pragma once

#include "iscene.h"
#include "../camera_collector.hpp"

#include <motor/scene/node/logic_group.h>
#include <motor/scene/component/name_component.hpp>
#include <motor/scene/component/render_settings_component.h>
#include <motor/scene/component/trafo3d_component.h>

#include <motor/graphics/state/render_states.h>

#include <motor/tool/imgui/node_kit/imgui_node_visitor.h>

#include <motor/format/global.h>
#include <motor/log/global.h>
#include <motor/memory/global.h>
#include <motor/concurrent/global.h>

namespace demos
{

// deferes the on_* calls in order to
// test how other scenes react.
class defered_scene : public iscene
{
    motor_this_typedefs( defered_scene );

  public:

    defered_scene( motor::string_cref_t name ) noexcept : iscene( name ) {}
    defered_scene( defered_scene const & ) = delete;
    defered_scene( defered_scene && rhv ) noexcept : iscene( std::move( rhv ) ) {}
    virtual ~defered_scene( void_t ) noexcept
    {
        this_t::release_all_objects();
    }

  public:

    //************************************************************************************
    virtual void_t on_init( motor::io::database_ptr_t db ) noexcept
    {
        std::this_thread::sleep_for( std::chrono::milliseconds( 2000 ) );
    }

    //************************************************************************************
    virtual void_t on_release( void_t ) noexcept
    {
        this_t::release_all_objects();
    }

    //************************************************************************************
    virtual void_t on_update( demos::iscene::update_data_cref_t ) noexcept {}

    //************************************************************************************
    virtual void_t on_resize_debug( uint_t const width, uint_t const height ) noexcept {}

    //************************************************************************************
    virtual void_t on_resize( uint_t const width, uint_t const height ) noexcept {}

    //************************************************************************************
    virtual void_t on_graphics( demos::iscene::on_graphics_data_in_t ) noexcept {}

    //************************************************************************************
    virtual void_t on_render_init( demos::window_type const,
        motor::graphics::gen4::frontend_ptr_t fe,
        motor::graphics::gen4::frontend::fence_funk_t funk ) noexcept
    {
        auto the_task = motor::shared(
            motor::concurrent::task_t( [ = ]( motor::concurrent::task_t::task_funk_param const & )
        {
            std::this_thread::sleep_for( std::chrono::milliseconds( 2000 ) );
            funk();
        } ) );

        motor::concurrent::global::schedule(
            motor::move( the_task ), motor::concurrent::schedule_type::loose );

        // the fence func is supposed to be sent to the
        // rendering backend where it is called.
        // fe->fence( funk );
    }

    //************************************************************************************
    virtual void_t on_render_deinit( demos::window_type const,
        motor::graphics::gen4::frontend_ptr_t,
        motor::graphics::gen4::frontend::fence_funk_t funk ) noexcept
    {
        auto the_task = motor::shared(
            motor::concurrent::task_t( [ = ]( motor::concurrent::task_t::task_funk_param const & )
        {
            std::this_thread::sleep_for( std::chrono::milliseconds( 2000 ) );
            funk();
        } ) );

        motor::concurrent::global::schedule(
            motor::move( the_task ), motor::concurrent::schedule_type::loose );

        // the fence func is supposed to be sent to the
        // rendering backend where it is called.
        // fe->fence( funk );
    }

    //************************************************************************************
    virtual void_t on_render_debug(
        size_t const wid, motor::graphics::gen4::frontend_ptr_t ) noexcept
    {
    }
    virtual void_t on_render_final(
        size_t const wid, motor::graphics::gen4::frontend_ptr_t ) noexcept
    {
    }

    virtual void_t on_tool( void_t ) noexcept
    {
        ImGui::Text( "defered scene" );
    }

  private:

    void_t release_all_objects( void_t ) noexcept {}
};
} // namespace demos