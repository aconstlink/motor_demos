
#pragma once

#include <motor/gfx/camera/generic_camera.h>
#include <motor/gfx/primitive/primitive_render_3d.h>

#include <motor/graphics/frontend/gen4/frontend.hpp>


#include <motor/math/spline/linear_bezier_spline.hpp>
#include <motor/math/spline/cubic_hermit_spline.hpp>
#include <motor/math/animation/keyframe_sequence.hpp>

#include <motor/io/database.h>
#include <motor/base/types.hpp>

namespace demos
{
    using namespace motor::core::types ;

    

    class iscene
    {
        motor_this_typedefs( iscene ) ;

    public:

        struct on_graphics_data
        {
            size_t const cur_time; 
            float_t const dt ;
            motor::gfx::primitive_render_3d_ptr_t pr ;
        };
        motor_typedef( on_graphics_data ) ;

    private: 

        motor::string_t _name ;
        size_t const _start ;
        size_t const _end ;

    public:

        iscene( motor::string_in_t name, size_t const start, size_t const end ) noexcept : 
            _name( name ), _start( start ), _end( end ){}

        iscene( this_cref_t ) = delete ;
        iscene( this_rref_t rhv ) noexcept : _name( std::move( rhv._name ) ),
            _start( rhv._start ), _end( rhv._end )
        {}

        virtual ~iscene( void_t ) noexcept{}

    public: // 

        std::pair< size_t, size_t > get_time_range( void_t ) const noexcept
        {
            return std::make_pair( _start, _end ) ;
        }

        bool_t is_in_time_range( size_t const milli ) const noexcept
        {
            return milli >= _start && milli <= _end ;
        }

        motor::string_cref_t name( void_t ) const noexcept { return _name ; }


    public: // interface

        virtual void_t on_init( motor::io::database_ref_t ) noexcept = 0 ;
        virtual void_t on_release( void_t ) noexcept = 0 ;

        virtual void_t on_update( size_t const cur_time ) noexcept = 0 ;
        virtual void_t on_camera_debug( motor::gfx::generic_camera_ptr_t cam ) noexcept = 0 ;
        virtual void_t on_camera_final( motor::gfx::generic_camera_ptr_t cam ) noexcept = 0 ;

        virtual void_t on_graphics( demos::iscene::on_graphics_data_in_t ) noexcept = 0 ;

        virtual void_t on_render_debug( bool_t const initial, motor::graphics::gen4::frontend_ptr_t ) noexcept = 0 ;
        virtual void_t on_render_final( bool_t const initial,  motor::graphics::gen4::frontend_ptr_t ) noexcept = 0 ;

        virtual void_t on_tool( void_t ) noexcept = 0 ;
        virtual void_t on_shutdown( void_t ) noexcept = 0 ;

    };
    motor_typedef( iscene ) ;
}