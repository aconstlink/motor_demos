
#pragma once

#include "../../common/iscene.h"
#include <motor/concurrent/range_1d.hpp>

namespace demos
{
    using pos_funk_t = std::function< motor::math::vec3f_t ( float_t const t ) > ;

    class scene_1 : public demos::iscene
    {
        motor_this_typedefs( scene_1 ) ;

    private: // general

        motor::io::database_mtr_t _db = nullptr ;
        motor::io::monitor_mtr_t _mon = nullptr ;

        motor::vector< motor::graphics::msl_object_ptr_t > _reconfigs_debug ;
        motor::vector< motor::graphics::msl_object_ptr_t > _reconfigs_prod ;

    private: // 

        size_t _max_objects = 150000 ;
        size_t _num_objects = 1 ;

        size_t num_objects( void_t ) const noexcept { return _num_objects ; }
        void_t set_num_objects( size_t const no ) noexcept
        {
            _num_objects = std::min( no, _max_objects ) ;
        }

        size_t _num_sphere_cubes = 1 ;
        size_t num_sphere_cubes( void_t ) const noexcept
        {
            return _num_sphere_cubes ;
        }

        void_t set_num_sphere_cubes( size_t const s ) noexcept
        {
            _num_sphere_cubes = std::min( s, _max_objects ) ; ;
        }

    private: // cubes

        motor::graphics::geometry_object_mtr_t _cubes_geo = nullptr ;
        motor::graphics::array_object_mtr_t _cubes_data = nullptr ;

        motor::graphics::msl_object_mtr_t _cubes_lib_msl = nullptr ;
        motor::graphics::msl_object_mtr_t _cubes_debug_msl = nullptr ;
        motor::graphics::msl_object_mtr_t _cubes_final_msl = nullptr ;

    private: // sphere

        motor::graphics::array_object_mtr_t _sphere_data = nullptr ;

        motor::graphics::msl_object_mtr_t _sphere_lib_msl = nullptr ;
        motor::graphics::msl_object_mtr_t _sphere_debug_msl = nullptr ;
        motor::graphics::msl_object_mtr_t _sphere_final_msl = nullptr ;

    private:// debug view

        motor::graphics::state_object_t _debug_rs ;

        // I think this is not needed.
        // always taking the dims from the render window
        motor::math::vec2ui_t _dbg_dims ;

    private: // final render

        motor::graphics::state_object_t _scene_final_rs ;
        motor::math::vec2ui_t _rnd_dims ;

    public:

        scene_1( motor::string_in_t name, demos::demo_mode const dm ) noexcept : iscene( name, dm )
        {

        }

        scene_1 ( this_rref_t rhv ) noexcept : iscene( std::move( rhv ) ),
            _debug_rs( std::move( rhv._debug_rs ) ), 
            _scene_final_rs( std::move( rhv._scene_final_rs ) )
            
        {
        }

        virtual ~scene_1( void_t ) noexcept
        {
        }

    public:

        virtual void_t on_init_cameras( void_t ) noexcept ;
        virtual void_t on_init( motor::io::database_ptr_t ) noexcept ;
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

        typedef struct
        {
            // x: sphere cube idx
            motor::math::vec4f_t idx ;
        } sphere_data_t ;

    private: // general properties

        size_t _width = 250 ;
        size_t _depth = 250 ;
        float_t _offset_width = 50.0f ;
        float_t _offset_depth = 50.0f ;
        float_t _cube_scale = 50.0f ;

    private: // point of interest properties

        bool_t _enable_poi = false ;
        float_t _poi_x = 0.0f ;
        float_t _poi_z = 0.0f ;
        float_t _falloff_poi = 10.0f ;

    private: // line lift properties 

        bool_t _x_line_lift_enable = false ;
        int_t _x_line_lift = 0 ;
        
    private: // wave properties 

        bool_t _enable_wave = true ;
        float_t _wave_freq = 1.0f ;
        float_t _wave_phase = 0.0f ;
        float_t _wave_amp = 1.0f ;

    private: // lift point properties 

        bool_t _enable_lift_points = false ;
        float_t _lp_falloff_thres = 5.0f ;

        motor::vector< motor::math::vec2f_t > _lift_points ;
        void_t randomize_lift_points( void_t ) noexcept 
        {
            _lift_points.resize( 20 ) ;
            for( size_t i=0; i<_lift_points.size(); ++i )
            {
                _lift_points[i] = motor::math::vec2f_t( 
                    this_t::rand(i*3+1), this_t::rand(i*2+1) ) *
                    motor::math::vec2f_t( float_t(_width), float_t(_depth) ) ;
            }
        }

        #if 0
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

        size_t _ring_rotate = size_t( -1 ) ;
        size_t _ring_to_lift = size_t( -1 ) ;
        size_t _ring_to_lift_range = size_t( 0 ) ;
        float_t _ring_lift_radius = 0.0f ;

        size_t num_rings( void_t ) const noexcept { return _num_rings ; }
        #endif

        

        typedef motor::concurrent::range_1d<size_t> range_t ;

        void_t coreo_0( demos::iscene::on_graphics_data_in_t ) noexcept ;

        void_t coreo_1( demos::iscene::on_graphics_data_in_t ) noexcept ;

    private: // random numbers

        std::array< float_t, 1024 > _random_numbers ;
        float_t rand( size_t const i ) const noexcept
        {
            return _random_numbers[ i % _random_numbers.size() ] ;
        }
    };
}