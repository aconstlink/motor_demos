
#pragma once

#include "../../common/iscene.h"

namespace demos
{
    class scene_0 : public demos::iscene 
    {
        motor_this_typedefs( scene_0 ) ;

    private: // cubes

        int_t _max_objects = 10000 ;

        motor::graphics::geometry_object_t _cubes_geo ;
        motor::graphics::array_object_t _cubes_data ;

        motor::graphics::msl_object_t _cubes_debug_msl ;
        motor::graphics::msl_object_t _cubes_final_msl ;

        typedef motor::math::cubic_hermit_spline< motor::math::vec3f_t > vec3splinef_t ;
        typedef motor::math::keyframe_sequence< vec3splinef_t > keyframe_sequencef_t ;
        keyframe_sequencef_t _worm_pos ;
        vec3splinef_t _worm_pos_spline ;
        


    private:// debug view

        motor::graphics::state_object_t _debug_rs ;
        motor::graphics::msl_object_mtr_t _dummy_debug_msl ;

        // I think this is not needed.
        // always taking the dims from the render window
        motor::math::vec2ui_t _dbg_dims ;

    private: // final render

        motor::graphics::state_object_t _scene_final_rs ;
        motor::graphics::msl_object_mtr_t _dummy_render_msl ;
        

        motor::graphics::geometry_object_mtr_t _dummy_geo ;
        

        motor::math::vec2ui_t _rnd_dims ;

    public:

        scene_0( motor::string_in_t name, size_t const s, size_t const e ) noexcept : iscene( name, s, e )
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

        virtual void_t on_init( motor::io::database_ref_t ) noexcept ;
        virtual void_t on_release( void_t ) noexcept ;

        virtual void_t on_resize_debug( uint_t const width, uint_t const height ) noexcept ;
        virtual void_t on_resize( uint_t const width, uint_t const height ) noexcept ;

        virtual void_t on_update( size_t const cur_time ) noexcept ;
        virtual void_t on_graphics( demos::iscene::on_graphics_data_in_t ) noexcept  ;

        virtual void_t on_render_debug( bool_t const initial, motor::graphics::gen4::frontend_ptr_t ) noexcept ;
        virtual void_t on_render_final( bool_t const initial, motor::graphics::gen4::frontend_ptr_t ) noexcept ;

        virtual void_t on_tool( void_t ) noexcept ;
    };
}