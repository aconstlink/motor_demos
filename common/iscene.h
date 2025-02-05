
#pragma once



#include "camera_manager.h"

#include <motor/graphics/frontend/gen4/frontend.hpp>
#include <motor/io/database.h>
#include <motor/concurrent/task/task.hpp>

namespace demos
{
    using namespace motor::core::types ;

    //**********************************************************
    // Base scene
    class iscene
    {
        motor_this_typedefs( iscene ) ;

    public:

        enum class render_mode
        {
            debug,
            production
        };
        
    public:

        struct on_graphics_data
        {
            size_t const cur_time; 
            float_t const dt ;
            motor::gfx::primitive_render_3d_ptr_t pr ;
            motor::gfx::generic_camera_mtr_t dbg_cam ;
        };
        motor_typedef( on_graphics_data ) ;

    private: 

        motor::string_t _name ;
        size_t const _start ;
        size_t const _end ;
        // final rendering
        bool_t _scene_is_on = false ;
        
        demos::camera_manager _cm ;

    public: 

        // camera manager
        demos::camera_manager & camera_manager( void_t ) noexcept { return _cm ; }

    public:

        iscene( motor::string_in_t name, size_t const start, size_t const end ) noexcept : 
            _name( name ), _start( start ), _end( end ){}

        iscene( this_cref_t ) = delete ;
        iscene( this_rref_t rhv ) noexcept : _name( std::move( rhv._name ) ),
            _start( rhv._start ), _end( rhv._end )
        {}

        virtual ~iscene( void_t ) noexcept{}

    public: // 
        
        // should be used for final rendition
        bool_t is_in_time_range( void_t ) const noexcept
        {
            return _scene_is_on ;
        }

        std::pair< size_t, size_t > get_time_range( void_t ) const noexcept
        {
            return std::make_pair( _start, _end ) ;
        }

        bool_t is_in_time_range( size_t const milli ) const noexcept
        {
            return milli >= _start && milli <= _end ;
        }

        motor::string_cref_t name( void_t ) const noexcept { return _name ; }
        
    protected:

        void_t update_time( size_t const cur_time ) noexcept
        {
            _scene_is_on = this_t::is_in_time_range( cur_time ) ;
        }

    public: // interface

        virtual void_t on_init( motor::io::database_ptr_t ) noexcept = 0 ;
        virtual void_t on_release( void_t ) noexcept = 0 ;

        virtual void_t on_update( size_t const cur_time ) noexcept = 0 ;

        virtual void_t on_resize_debug( uint_t const width, uint_t const height ) noexcept = 0 ;
        virtual void_t on_resize( uint_t const width, uint_t const height ) noexcept = 0 ;

        virtual void_t on_graphics( demos::iscene::on_graphics_data_in_t ) noexcept = 0 ;

        virtual void_t on_render_init( demos::iscene::render_mode const, motor::graphics::gen4::frontend_ptr_t ) noexcept = 0 ;
        virtual void_t on_render_deinit( demos::iscene::render_mode const, motor::graphics::gen4::frontend_ptr_t ) noexcept = 0 ;
        virtual void_t on_render_debug( motor::graphics::gen4::frontend_ptr_t ) noexcept = 0 ;
        virtual void_t on_render_final( motor::graphics::gen4::frontend_ptr_t ) noexcept = 0 ;

        virtual void_t on_tool( void_t ) noexcept = 0 ;

    };
    motor_typedef( iscene ) ;
}