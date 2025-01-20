

#pragma once

#include <motor/base/types.hpp>
#include <motor/math/vector/vector2.hpp>
#include <motor/math/vector/vector3.hpp>
#include <motor/math/utility/3d/transformation.hpp>

#include <motor/controls/types/ascii_keyboard.hpp>
#include <motor/controls/types/three_mouse.hpp>


namespace helper
{
    using namespace motor::core::types ;

    class camera_controls
    {
        motor_this_typedefs( camera_controls ) ;

    public:

        struct control_vector
        {
            bool_t move_left = false ;
            bool_t move_right = false ;
            bool_t move_forwards = false ;
            bool_t move_backwards = false ;
            bool_t move_upwards = false ;
            bool_t move_downwards = false ;

            motor::math::vec2f_t mouse_coords ;

            int_t rotate_x = 0 ;
            int_t rotate_y = 0 ;
            int_t rotate_z = 0 ;
        };

    public:

        //**********************************************************************************
        static void_t test_controls( motor::controls::ascii_device_ptr_t ascii, 
            motor::controls::three_device_ptr_t mptr, this_t::control_vector & cv ) noexcept
        {
            bool_t ctrl = false ;

            {
                motor::controls::types::ascii_keyboard_t keyboard( ascii ) ;

                using layout_t = motor::controls::types::ascii_keyboard_t ;
                using key_t = layout_t::ascii_key ;

                auto const left = keyboard.get_state( key_t::a ) ;
                auto const right = keyboard.get_state( key_t::d ) ;
                auto const forw = keyboard.get_state( key_t::w ) ;
                auto const back = keyboard.get_state( key_t::s ) ;
                auto const asc = keyboard.get_state( key_t::q ) ;
                auto const dsc = keyboard.get_state( key_t::e ) ;

                cv.move_left = left != motor::controls::components::key_state::none ;
                cv.move_right = right != motor::controls::components::key_state::none ;
                cv.move_forwards = forw != motor::controls::components::key_state::none ;
                cv.move_backwards = back != motor::controls::components::key_state::none ;
                cv.move_upwards = asc != motor::controls::components::key_state::none ;
                cv.move_downwards = dsc != motor::controls::components::key_state::none ;

                ctrl = keyboard.get_state( key_t::ctrl_left ) !=
                    motor::controls::components::key_state::none ;
            }

            // mouse testing
            {
                motor::controls::types::three_mouse_t mouse( mptr ) ;

                motor::math::vec2f_t const mouse_coords = mouse.get_local() ;
                auto const dif = mouse_coords - cv.mouse_coords ;
                cv.mouse_coords = mouse_coords ;

                auto button_funk = [&] ( motor::controls::types::three_mouse_t::button const button )
                {
                    if ( mouse.is_pressed( button ) )
                    {
                        return true ;
                    }
                    else if ( mouse.is_pressing( button ) )
                    {
                        return true ;
                    }
                    else if ( mouse.is_released( button ) )
                    {
                    }
                    return false ;
                } ;

                auto const l = button_funk( motor::controls::types::three_mouse_t::button::left ) ;
                auto const r = button_funk( motor::controls::types::three_mouse_t::button::right ) ;
                auto const m = button_funk( motor::controls::types::three_mouse_t::button::middle ) ;

                cv.rotate_x = r ? int_t( -motor::math::fn<float_t>::sign( dif.y() ) ) : 0 ;
                cv.rotate_y = r ? int_t( +motor::math::fn<float_t>::sign( dif.x() ) ) : 0 ;
                cv.rotate_z = ctrl ? int_t( +motor::math::fn<float_t>::sign( dif.x() ) ) : 0 ;
            }
        }

        //**********************************************************************************
        static motor::math::vec3f_t create_translation( float_t const dt, 
            this_t::control_vector const & cv ) noexcept
        {
            motor::math::vec3f_t translate ;

            // left/right
            {
                if ( cv.move_left )
                {
                    translate.x( -1000.0f * dt ) ;
                }
                else if ( cv.move_right )
                {
                    translate.x( +1000.0f * dt ) ;
                }
            }

            // forwards/backwards
            {
                if ( cv.move_backwards )
                {
                    translate.z( -1000.0f * dt ) ;
                }
                else if ( cv.move_forwards )
                {
                    translate.z( +1000.0f * dt ) ;
                }
            }

            // upwards/downwards
            {
                if ( cv.move_upwards )
                {
                    translate.y( -1000.0f * dt ) ;
                }
                else if ( cv.move_downwards )
                {
                    translate.y( +1000.0f * dt ) ;
                }
            }

            return translate ;
        }

        //**********************************************************************************
        static motor::math::m3d::trafof_t create_rotation( float_t const dt,
            this_t::control_vector const & cv ) noexcept
        {
            motor::math::vec3f_t const angle(
                float_t( cv.rotate_x ) * 2.0f * dt,
                float_t( cv.rotate_y ) * 2.0f * dt,
                float_t( cv.rotate_z ) * 2.0f * dt ) ;

            motor::math::quat4f_t const x( angle.x(), motor::math::vec3f_t( 1.0f, 0.0f, 0.0f ) ) ;
            motor::math::quat4f_t const y( angle.y(), motor::math::vec3f_t( 0.0f, 1.0f, 0.0f ) ) ;
            motor::math::quat4f_t const z( angle.z(), motor::math::vec3f_t( 0.0f, 0.0f, 1.0f ) ) ;

            auto const final_axis = x * y * z ;

            return motor::math::m3d::trafof_t::rotation_by_matrix( final_axis.to_matrix() ) ;
        }
    };
}