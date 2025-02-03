


#pragma once

#include <motor/gfx/camera/generic_camera.h>
#include <motor/gfx/primitive/primitive_render_3d.h>

#include <motor/math/spline/linear_bezier_spline.hpp>
#include <motor/math/spline/cubic_hermit_spline.hpp>
#include <motor/math/animation/keyframe_sequence.hpp>


#include <motor/base/types.hpp>

namespace demos
{
    using namespace motor::core::types ;

    struct store_camera_data
    {
        motor::math::vec3f_t pos ;
        motor::math::vec3f_t lookat ;
    };
    using store_camera_datas_t = motor::vector< store_camera_data >  ;

    struct camera_data
    {
        typedef motor::math::linear_bezier_spline< motor::math::vec3f_t > linearf_t ;
        typedef motor::math::cubic_hermit_spline< motor::math::vec3f_t > splinef_t ;

        typedef motor::math::keyframe_sequence< splinef_t > keyframe_sequencef_t ;

        motor::gfx::generic_camera_t cam  ;
        keyframe_sequencef_t kf_pos ;
        keyframe_sequencef_t kf_lookat ;
    };

    struct free_camera
    {
        motor::math::vec3f_t pos ;
        motor::math::vec3f_t lookat ;
    };

    class camera_manager
    {
        motor_this_typedefs( camera_manager ) ;

    public:

        typedef motor::math::linear_bezier_spline< size_t > linear_t ;
        typedef motor::math::keyframe_sequence< linear_t > camera_kfs_t ;

    private:

        demos::free_camera _free_cam ;
        motor::vector< demos::camera_data > _cameras ;

        // the debug window camera
        size_t _cam_idx = 0 ;

        // the render window camera
        size_t _final_cam_idx = 1 ;

    private:

        camera_kfs_t _cam_selector ;

    public: // camera controls

        // this is only for the storage functionality.
        void_t set_free_camera( demos::free_camera const & cd ) noexcept 
        {
            _free_cam = cd ;
        }

        void_t set_camera_selector( camera_kfs_t && kfs ) noexcept
        {
            _cam_selector = std::move( kfs ) ;
        }

        void_t add_camera( demos::camera_data && cd ) noexcept
        {
            _cameras.emplace_back( std::move( cd ) ) ;
        }

        // moves the camera based on the current time along its keyframes
        // @param cur_time the current time played
        // @param dbg_cam overwrite the dbg camera idx. -1 is not overwriting
        void_t update_camera( size_t const cur_time, size_t const dbg_cam = size_t( -1 ) ) noexcept
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
            if( _cameras.size() != 0 )
            {
                _final_cam_idx = _cam_selector( cur_time ) ;
                _cam_idx = dbg_cam != size_t( -1 ) ? dbg_cam : _final_cam_idx ;
            }
        }

        motor::gfx::generic_camera_ptr_t borrow_cur_debug_camera( void_t ) noexcept
        {
            return &( _cameras[ _cam_idx ].cam ) ;
        }

    public: // public camera controls

        size_t get_num_cameras( void_t ) const noexcept
        {
            return _cameras.size() ;
        }

        bool_t get_camera_data( size_t const idx, demos::camera_data & cd ) const noexcept
        {
            if( idx >= this_t::get_num_cameras() ) return false ;
            cd = _cameras[idx] ;
            return true ;
        }

        using foreach_camera_funk_t = std::function< void_t ( size_t const idx, demos::camera_data & ) > ;
        void_t for_each_camera( foreach_camera_funk_t funk ) noexcept
        {
            size_t i = size_t( -1 ) ;
            for ( auto & c : _cameras ) funk( ++i, c ) ;
        }

        size_t get_debug_camera_idx( void_t ) const noexcept { return _cam_idx ; }
        size_t get_final_camera_idx( void_t ) const noexcept { return _final_cam_idx ; }

        motor::gfx::generic_camera_ptr_t borrow_debug_camera( void_t ) noexcept { return &( _cameras[ _cam_idx ].cam ) ; }
        motor::gfx::generic_camera_ptr_t borrow_final_camera( void_t ) noexcept { return &( _cameras[ _final_cam_idx ].cam ) ; }


    private:

        // used for writing to a file
        store_camera_datas_t _storage ;

    public:

        void_t append_current_free_camera( void_t ) noexcept ;
        void_t append( store_camera_data && ss  ) noexcept;
        void_t for_each( std::function< void_t ( store_camera_data const & ) > f ) noexcept ;
        void_t clear_storage_data( void_t ) noexcept ;
    };
    motor_typedef( camera_manager ) ;
}