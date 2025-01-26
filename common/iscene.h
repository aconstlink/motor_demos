
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

    struct camera_data
    {
        typedef motor::math::linear_bezier_spline< motor::math::vec3f_t > linearf_t ;
        typedef motor::math::cubic_hermit_spline< motor::math::vec3f_t > splinef_t ;

        typedef motor::math::keyframe_sequence< splinef_t > keyframe_sequencef_t ;

        motor::gfx::generic_camera_t cam  ;
        keyframe_sequencef_t kf_pos ;
        keyframe_sequencef_t kf_lookat ;
    };

    //**********************************************************
    // Base scene
    class iscene
    {
        motor_this_typedefs( iscene ) ;

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

        motor::vector< demos::camera_data > _cameras ;
        
        // the debug window camera
        size_t _cam_idx = 0 ;

        // the render window camera
        size_t _final_cam_idx = 1 ;

    protected: 

        typedef motor::math::linear_bezier_spline< size_t > linear_t ;
        typedef motor::math::keyframe_sequence< linear_t > camera_kfs_t ;

    private:

        camera_kfs_t _cam_selector ;

    protected:

        void_t set_camera_selector( camera_kfs_t && kfs ) noexcept
        {
            _cam_selector = std::move( kfs ) ;
        }

        void_t add_camera( demos::camera_data && cd ) noexcept 
        {
            _cameras.emplace_back( std::move( cd ) ) ;
        }

        void_t update_camera( size_t const cur_time ) noexcept
        {
            // move the camera on the path
            {
                for ( size_t c = 0; c < _cameras.size(); ++c )
                {
                    auto & cam = _cameras[ c ] ;

                    motor::math::vec3f_t const up = motor::math::vec3f_t( 0.0f, 1.0f, 0.0f ).normalized() ;
                    cam.cam.look_at( cam.kf_pos( cur_time ), up, cam.kf_lookat( cur_time ) ) ;
                }
            }

            _cam_idx = _cam_selector( cur_time ) ;
            _final_cam_idx = _cam_idx ;
        }

        motor::gfx::generic_camera_ptr_t borrow_cur_debug_camera( void_t ) noexcept 
        {
            return &(_cameras[_cam_idx].cam) ;
        }

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

        using foreach_camera_funk_t = std::function< void_t ( size_t const idx, demos::camera_data & ) > ;
        void_t for_each_camera( foreach_camera_funk_t funk ) noexcept
        {
            size_t i = size_t( -1 ) ;
            for ( auto & c : _cameras ) funk( ++i, c ) ;
        }

        size_t get_debug_camera_idx( void_t ) const noexcept { return _cam_idx ; }
        size_t get_final_camera_idx( void_t ) const noexcept { return _final_cam_idx ; }

        motor::gfx::generic_camera_ptr_t borrow_debug_camera( void_t ) noexcept { return &(_cameras[_cam_idx].cam) ; }
        motor::gfx::generic_camera_ptr_t borrow_final_camera( void_t ) noexcept { return &(_cameras[_final_cam_idx].cam) ; }

    public: // interface

        virtual void_t on_init( motor::io::database_ref_t ) noexcept = 0 ;
        virtual void_t on_release( void_t ) noexcept = 0 ;

        virtual void_t on_update( size_t const cur_time ) noexcept = 0 ;

        virtual void_t on_resize_debug( uint_t const width, uint_t const height ) noexcept = 0 ;
        virtual void_t on_resize( uint_t const width, uint_t const height ) noexcept = 0 ;

        virtual void_t on_graphics( demos::iscene::on_graphics_data_in_t ) noexcept = 0 ;

        virtual void_t on_render_debug( bool_t const initial, motor::graphics::gen4::frontend_ptr_t ) noexcept = 0 ;
        virtual void_t on_render_final( bool_t const initial,  motor::graphics::gen4::frontend_ptr_t ) noexcept = 0 ;

        virtual void_t on_tool( void_t ) noexcept = 0 ;
        virtual void_t on_shutdown( void_t ) noexcept = 0 ;

    };
    motor_typedef( iscene ) ;
}