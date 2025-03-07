
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

    // determine ijk cube indices
    #if 1
    {
        static bool_t recompute = true ;
        if ( recompute )
        {
            size_t width = 100 ;
            size_t height = 100 ;
            size_t depth = 100 ;
            int_t radius = 25 ;

            size_t const num_elems = width * height * depth ;

            size_t req_items = 0 ;

            auto compute_ijk = []( size_t const i, size_t const width, size_t const height, size_t const depth )
            {
                return motor::math::vector3<size_t>( 
                    i / 1 % width, 
                    i / width % height,
                    i / (width*height) % depth
                ) ;
            } ;

            auto compute_xyz = [=]( size_t const i, size_t const width, size_t const height, size_t const depth )
            {
                return motor::math::vec3i_t( compute_ijk( i, width, height, depth ) ) -
                    motor::math::vec3i_t( int_t(width/2), int_t(height/2), int_t(depth/2) ) ;
            } ;

            auto is_on_sphere = [=]( size_t const i, size_t const width, size_t const height, size_t const depth, int_t const radius, motor::math::vec3i_ref_t vo )
            {
                auto const xyz = compute_xyz( i, width, height, depth ) ;
                vo = xyz ;
                return xyz.length2() == (radius*radius) ;
            } ;

            for( size_t i=0; i<num_elems; ++i )
            {
                motor::math::vec3i_t v ;
                if( is_on_sphere( i, width, height, depth, radius, v  ) )
                {
                    ++req_items ;
                }
            }

            std::atomic< size_t > idx = size_t(-1) ;

            _sphere_data->data_buffer().resize( req_items ).
                update< sphere_data_t >( [&] ( sphere_data_t * array, size_t const ne )
            {
                typedef motor::concurrent::range_1d<size_t> range_t ;
                auto const & range = range_t( 0, num_elems ) ;

                motor::concurrent::parallel_for<size_t>( range, [&] ( range_t const & range )
                {
                    for( size_t r = range.begin(); r<range.end(); ++r )
                    {
                        motor::math::vec3i_t vo ;
                        if( is_on_sphere( r, width, height, depth, radius, vo ) )
                        {
                            array[++idx].idx = motor::math::vec4f_t( float_t(vo.x()), 
                                float_t(vo.y()), float_t(vo.z()), 0.0f ) ;
                        }
                    }
                } ) ;
            } ) ;

            this_t::set_num_sphere_cubes( req_items ) ;

            recompute = false ;
        }
    }
    #endif
    
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