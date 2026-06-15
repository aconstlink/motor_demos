#pragma once

#include "iscene.h"

namespace demos
{
class dummy_scene : public iscene
{

  public:

    dummy_scene( motor::string_cref_t name ) noexcept : iscene( name ) {}
    dummy_scene( dummy_scene const & ) = delete;
    dummy_scene( dummy_scene && rhv ) noexcept : iscene( std::move( rhv ) ){}
    virtual ~dummy_scene( void_t ) noexcept {}

  public:

    virtual void_t on_init_cameras( void_t ) noexcept {}
    virtual void_t on_init( motor::io::database_ptr_t ) noexcept {}
    virtual void_t on_release( void_t ) noexcept {}

    virtual void_t on_update( size_t const cur_time ) noexcept {}

    virtual void_t on_resize_debug( uint_t const width, uint_t const height ) noexcept {}
    virtual void_t on_resize( uint_t const width, uint_t const height ) noexcept {}

    virtual void_t on_graphics( demos::iscene::on_graphics_data_in_t ) noexcept {}

    virtual void_t on_render_init( demos::iscene::render_mode const,
                                   motor::graphics::gen4::frontend_ptr_t ) noexcept
    {
    }
    virtual void_t on_render_deinit( demos::iscene::render_mode const,
                                     motor::graphics::gen4::frontend_ptr_t ) noexcept
    {
    }
    virtual void_t on_render_debug( motor::graphics::gen4::frontend_ptr_t ) noexcept {}
    virtual void_t on_render_final( motor::graphics::gen4::frontend_ptr_t ) noexcept {}

    virtual void_t on_tool( void_t ) noexcept {}
};
} // namespace demos