
#pragma once



#include "camera_manager.h"

#include <motor/graphics/frontend/gen4/frontend.hpp>
#include <motor/io/database.h>
#include <motor/concurrent/task/task.hpp>

namespace demos
{
    using namespace motor::core::types ;

    enum class demo_mode
    {
        tool,
        production
    };

    //**********************************************************
    // Base scene
    class iscene
    {
        motor_this_typedefs( iscene ) ;

    public:

        enum class render_mode
        {
            tool,
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

        // final rendering
        bool_t _scene_is_on = false ;
        
        demos::camera_manager _cm ;

        demos::demo_mode _dm ;

    public: 

        // camera manager
        demos::camera_manager & camera_manager( void_t ) noexcept { return _cm ; }

    public:

        iscene( motor::string_in_t name, demos::demo_mode const dm ) noexcept : 
            _name( name ), _dm( dm ) {}

        iscene( this_cref_t ) = delete ;
        iscene( this_rref_t rhv ) noexcept : _name( std::move( rhv._name ) ),
        _dm( rhv._dm ) {}

        virtual ~iscene( void_t ) noexcept{}

    public: // 
        
        // should be used for final rendition
        bool_t is_in_time_range( void_t ) const noexcept
        {
            return _scene_is_on ;
        }

        std::pair< size_t, size_t > get_time_range( void_t ) const noexcept
        {
            return _cm.camera_selector_range() ;
        }

        bool_t is_in_time_range( size_t const milli ) const noexcept
        {
            std::pair< size_t, size_t > range ;
            if( !_cm.camera_selector_range( range ) ) return false ;
            return milli >= range.first && milli <= range.second ;
        }

        motor::string_cref_t name( void_t ) const noexcept { return _name ; }
        
    public: // preload time range

        // should be used for final rendition
        bool_t is_in_preload_time_range( size_t const cur_time ) const noexcept
        {
            size_t const preload_time = 2000 ;

            auto tr = this_t::get_time_range() ;

            auto const s = std::max( tr.first, size_t(preload_time) ) - preload_time ;
            auto const e = tr.second + preload_time ;

            return s <= cur_time && cur_time <= e ;
        }

    protected:

        void_t update_time( size_t const cur_time ) noexcept
        {
            _scene_is_on = this_t::is_in_time_range( cur_time ) ;
        }

        bool_t is_production_mode( void_t ) const noexcept
        {
            return _dm == demos::demo_mode::production ;
        }

        bool_t is_tool_mode( void_t ) const noexcept
        {
            return _dm == demos::demo_mode::tool ;
        }

    public: // interface

        virtual void_t on_init_cameras( void_t ) noexcept = 0 ;
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