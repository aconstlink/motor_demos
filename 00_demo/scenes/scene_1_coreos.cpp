
#include "scene_1.h"

#include <motor/math/vector/vector2.hpp>
#include <motor/math/utility/fn.hpp>
#include <motor/math/utility/angle.hpp>
#include <motor/concurrent/parallel_for.hpp>

using namespace demos ;

//*****************************************************************************************
void_t scene_1::coreo_0( demos::iscene::on_graphics_data_in_t gd ) noexcept
{
    _cubes_data->data_buffer().resize( this_t::num_objects() ).
        update< the_data >( [&] ( the_data * array, size_t const ne )
    {
        typedef motor::concurrent::range_1d<size_t> range_t ;
        auto const & range = range_t( 0, std::min( size_t( this_t::num_objects() ), this_t::num_objects() ) ) ;

        motor::concurrent::parallel_for<size_t>( range, [&] ( range_t const & range )
        {
            float_t const circle = 2.0f * motor::math::constants<float_t>::pi() ;

            for( size_t r = range.begin(); r<range.end(); ++r )
            {
                size_t const idx_x = r % _width ;
                size_t const idx_z = r / _width % _depth ;
                
                float_t y = 0.0f ;

                if( _enable_poi )
                {
                    auto const center = motor::math::vec2f_t( _poi_x, _poi_z ) ;
                    auto const pos = motor::math::vec2f_t( float_t(idx_x), float_t(idx_z) ) ;

                    auto const dist = ( pos - center ) ;

                    if( dist.length2() < (_falloff_poi * _falloff_poi) )
                    {
                        auto const falloff = float_t(_falloff_poi - dist.length())/float_t(_falloff_poi) ;
                        y += motor::math::interpolation<float_t>::linear( 0.0f, 200.0f, falloff ) ;
                    }
                }

                if( _enable_wave )
                {
                    {
                        //float_t const amp_rand = this_t::rand( idx_x / 2 ) * 2.0f - 1.0f ;
                        float_t const f = float_t(idx_x) / float_t( _width ) ;
                        //y += _wave_amp * motor::math::fn<float_t>::sin( _wave_freq * f * circle + _wave_phase ) ;
                    }

                    {
                        float_t const f = float_t(idx_z) / float_t( _depth ) ;
                        //y += _wave_amp * motor::math::fn<float_t>::cos( _wave_freq * f * circle + _wave_phase ) ;
                    }
                }

                if( _enable_lift_points )
                {
                    for( size_t i=0; i<_lift_points.size(); ++i )
                    {
                        auto const center = _lift_points[i] ;
                        auto const pos = motor::math::vec2f_t( float_t(idx_x), float_t(idx_z) ) ;

                        auto const dist = ( pos - center ) ;

                        float_t const thres = _lp_falloff_thres ;
                        if( dist.length2() < (thres*thres) )
                        {
                            auto const falloff = float_t(thres - dist.length())/float_t(thres) ;
                            y += motor::math::interpolation<float_t>::linear( 0.0f, 500.0f, falloff ) ;
                        }
                    }
                }

                auto const pos = motor::math::vec3f_t( float_t(idx_x), y, float_t(idx_z) ) * 
                    motor::math::vec3f_t( _offset_width, 1.0f, _offset_depth ) ;

                array[r].pos = motor::math::vec4f_t( pos, _cube_scale) ;
                array[r].col = motor::math::vec4f_t( 1.0f, 1.0f, 1.0f, 1.0f ) ;
                array[r].frame_x = motor::math::vec4f_t( 1.0f, 0.0f, 0.0f, 0.0f ) ;
                array[r].frame_y = motor::math::vec4f_t( 0.0f, 1.0f, 0.0f, 0.0f ) ;
                array[r].frame_z = motor::math::vec4f_t( 0.0f, 0.0f, 1.0f, 0.0f ) ;
            }
        } ) ;
    } ) ;
}

//*****************************************************************************************
void_t scene_1::coreo_1( demos::iscene::on_graphics_data_in_t gd ) noexcept
{
    _cubes_data->data_buffer().resize( this_t::num_objects() ).
        update< the_data >( [&] ( the_data * array, size_t const ne )
    {
        typedef motor::concurrent::range_1d<size_t> range_t ;
        auto const & range = range_t( 0, std::min( size_t( this_t::num_objects() ), this_t::num_objects() ) ) ;

        motor::concurrent::parallel_for<size_t>( range, [&] ( range_t const & r )
        {
        } ) ;
    } ) ;
}