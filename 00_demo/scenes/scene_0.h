
#pragma once

#include "../../common/iscene.h"
#include <motor/concurrent/range_1d.hpp>

namespace demos
{
    using pos_funk_t = std::function< motor::math::vec3f_t ( float_t const t ) > ;

    class scene_0 : public demos::iscene 
    {
        motor_this_typedefs( scene_0 ) ;

    private: // cubes

        size_t _max_objects = 50000 ;
        size_t _num_objects = 0 ;

        motor::graphics::geometry_object_t _cubes_geo ;
        motor::graphics::array_object_t _cubes_data ;

        motor::graphics::msl_object_mtr_t _cubes_debug_msl = nullptr;
        motor::graphics::msl_object_mtr_t _cubes_final_msl = nullptr;

        typedef motor::math::cubic_hermit_spline< motor::math::vec3f_t > vec3splinef_t ;
        typedef motor::math::keyframe_sequence< vec3splinef_t > keyframe_sequencef_t ;
        keyframe_sequencef_t _worm_pos ;
        vec3splinef_t _worm_pos_spline ;

        size_t _cur_pos_funk = 0 ;
        motor::vector< pos_funk_t > _pos_funks ;

        motor::math::vec3f_t pos_funk( float_t const t ) const noexcept 
        {
            return _pos_funks[_cur_pos_funk]( t ) ;
        }

        size_t num_objects( void_t ) const noexcept { return _num_objects ; }
        void_t set_num_objects( size_t const no ) noexcept 
        {
            _num_objects = std::min( no, _max_objects ) ;
        }
    private:// debug view

        motor::graphics::state_object_t _debug_rs ;
        motor::graphics::msl_object_mtr_t _dummy_debug_msl = nullptr ;

        // I think this is not needed.
        // always taking the dims from the render window
        motor::math::vec2ui_t _dbg_dims ;

    private: // final render

        motor::graphics::state_object_t _scene_final_rs ;
        motor::graphics::msl_object_mtr_t _dummy_render_msl = nullptr;
        

        motor::graphics::geometry_object_mtr_t _dummy_geo = nullptr;
        

        motor::math::vec2ui_t _rnd_dims ;

    public:

        scene_0( motor::string_in_t name, demos::demo_mode const dm ) noexcept : iscene( name, dm )
        {

        }

        scene_0 ( this_rref_t rhv ) noexcept : iscene( std::move( rhv ) ), 
            _debug_rs( std::move( rhv._debug_rs ) ), _dummy_debug_msl( motor::move( rhv._dummy_debug_msl )),
            _scene_final_rs( std::move( rhv._scene_final_rs ) ), _dummy_render_msl( motor::move( rhv._dummy_render_msl )),
            _dummy_geo( motor::move( rhv._dummy_geo ))
        {
        }

        virtual ~scene_0( void_t ) noexcept
        {
        }

    public:

        virtual void_t on_init_cameras( void_t ) noexcept ;
        virtual void_t on_init( motor::io::database_mtr_t ) noexcept ;
        virtual void_t on_release( void_t ) noexcept ;

        virtual void_t on_resize_debug( uint_t const width, uint_t const height ) noexcept ;
        virtual void_t on_resize( uint_t const width, uint_t const height ) noexcept ;

        virtual void_t on_update( size_t const cur_time ) noexcept ;
        virtual void_t on_graphics( demos::iscene::on_graphics_data_in_t ) noexcept  ;

        virtual void_t on_render_init( demos::iscene::render_mode const, motor::graphics::gen4::frontend_ptr_t ) noexcept ;
        virtual void_t on_render_deinit( demos::iscene::render_mode const, motor::graphics::gen4::frontend_ptr_t ) noexcept ;
        virtual void_t on_render_debug( motor::graphics::gen4::frontend_ptr_t ) noexcept ;
        virtual void_t on_render_final( motor::graphics::gen4::frontend_ptr_t ) noexcept ;

        virtual void_t on_tool( void_t ) noexcept ;

    private:

        struct the_data
        {
            motor::math::vec4f_t pos ;
            motor::math::vec4f_t col ;
            motor::math::vec4f_t frame_x ;
            motor::math::vec4f_t frame_y ;
            motor::math::vec4f_t frame_z ;
        };

    private:

        size_t _num_rings = 10 ;
        size_t _cubes_per_ring = 60 ;
        float_t _cube_radius = 10.0f ;
        size_t _per_ring_milli = 10 ;

        float_t _max_center_radius = 200.0f ;
        float_t _center_radius = 20.0f ;
        float_t _center_rand_radius = 0.0f ;
        float_t _center_rand_radius_mult = 0.0f ; // in [0.0,1.0]

        
        float_t _inner_amp = 20.0f ;
        float_t _inner_freq = 2.0f ;
        float_t _inner_shift = 0.0f ;
        float_t _direction_shift_rand_mult = 0.0f ;
        float_t _direction_offset = 0.0f ;
        bool_t _clamp_directionl_offset = true ;

        size_t _ring_rotate = size_t(-1) ;
        size_t _ring_to_lift = size_t(-1) ;
        size_t _ring_to_lift_range = size_t(0) ;
        float_t _ring_lift_radius = 0.0f ;

        std::array< float_t, 1024 > _random_numbers ;

        typedef motor::concurrent::range_1d<size_t> range_t ;

        // coreo_0 not used! Keeping just for copy and pasting
        void_t coreo_0( demos::iscene::on_graphics_data_in_t ) noexcept ;

        void_t coreo_1( demos::iscene::on_graphics_data_in_t ) noexcept ;

        size_t num_rings( void_t ) const noexcept { return _num_rings ; }
        float_t rand( size_t const i ) const noexcept 
        {
            return _random_numbers[i%_random_numbers.size()] ;
        }
    };
}