
#include "scene_0.h"

#include <motor/math/utility/fn.hpp>
#include <motor/math/utility/angle.hpp>
#include <motor/concurrent/parallel_for.hpp>

using namespace demos ;

//*****************************************************************************************
void_t scene_0::coreo_0( demos::iscene::on_graphics_data_in_t gd ) noexcept
{
    static float_t  angle_ = 0.0f ;
    angle_ += ( ( ( ( gd.dt ) ) ) * 2.0f * motor::math::constants<float_t>::pi() ) / 5.0f ;
    if ( angle_ > 4.0f * motor::math::constants<float_t>::pi() ) angle_ = 0.0f ;

    float_t s = 5.0f * std::sin( angle_ ) ;

    _cubes_data.data_buffer().resize( this_t::num_objects() ).
        update< the_data >( [&] ( the_data * array, size_t const ne )
    {
        typedef motor::concurrent::range_1d<size_t> range_t ;
        auto const & range = range_t( 0, std::min( size_t( this_t::num_objects() ), ne ) ) ;

        motor::concurrent::parallel_for<size_t>( range, [&] ( range_t const & r )
        {
            size_t const cubes_per_ring = _cubes_per_ring ;
            size_t const num_rings = ne / cubes_per_ring ;

            for ( size_t e = r.begin(); e < r.end(); ++e )
            {
                // the ring this item (e) is on.
                size_t const cur_ring = e / cubes_per_ring ;

                // the ring the time is on.
                size_t const cur_ring_time = ( gd.cur_time / 10 ) % num_rings ;

                float_t e0 = ( ( e ) % cubes_per_ring ) / float_t( cubes_per_ring ) ;
                //float_t e1 = float_t( cur_ring ) / (ne/cubes_per_ring) ;

                size_t const cur_idx = ( e / cubes_per_ring ) + 0 ;
                size_t const nxt_idx = ( e / cubes_per_ring ) + 1  ;


                float_t const t0 = float_t( cur_idx ) / float_t ( ne / cubes_per_ring ) ;
                float_t const t1 = float_t( nxt_idx ) / float_t ( ne / cubes_per_ring ) ;


                float_t const freq_mult = float_t( cur_ring % 2 ) * 2.0f - 1.0f ;

                float_t const freq_add = float_t ( ( gd.cur_time >> 2 ) % 1000 ) / 1000.0f ;
                float_t const circle = 2.0f * motor::math::constants<float_t>::pi() ;
                float_t const angle = motor::math::fn<float_t>::mod( e0 * circle + ( freq_add * freq_mult ), circle ) ;

                motor::math::vec3f_t final_pos ;

                {
                    auto const v0 = this_t::pos_funk( t0 ) ;
                    auto const v1 = this_t::pos_funk( t1 ) ;

                    auto dir = ( v1 - v0 ).normalized() ;
                    auto const up = motor::math::vec3f_t( 0.0f, 1.0f, 0.0f ) ;
                    auto const ortho_ = ( up - ( dir * dir.dot( up ) ) ).normalized() ;


                    motor::math::quat4f_t const axis( angle, dir ) ;
                    auto ortho = axis.to_matrix() * ortho_ ;
                    auto right = dir.crossed( ortho ).normalized().negated() ;

                    #if 0
                    float_t const rad_amp = 20.0f ;
                    float_t const radius = 300.0f ;
                    auto const radius_vec = ortho * radius ;
                    auto const base = v0 + radius_vec ;

                    #else
                    float_t const rad_amp = 50.0f ;
                    float_t const off_angle_0 = ( float_t( cur_ring + 0 ) / float_t( num_rings ) ) * circle * 5.0f;
                    float_t const off = rad_amp * motor::math::fn<float_t>::sin( off_angle_0 ) ;
                    float_t const rot_fix = off_angle_0;

                    float_t const radius = 300.0f + off ;
                    auto const radius_vec = ortho * radius ;
                    auto const base = v0 + radius_vec ;

                    {
                        motor::math::quat4f_t const axis_fix( rot_fix, dir ) ;
                        right = axis_fix * right ;
                        ortho = axis_fix * ortho ;
                    }
                    #endif

                    auto const pos = motor::math::vec4f_t( base, 20.0f ) ;
                    array[ e ].frame_x = motor::math::vec4f_t( right, 1.0f ) ;
                    array[ e ].frame_y = motor::math::vec4f_t( ortho, 1.0f ) ;
                    array[ e ].frame_z = motor::math::vec4f_t( dir, 1.0f ) ;

                    array[ e ].pos = pos ;
                }

                // determine color per ring
                {
                    motor::math::vec4f_t ring_colors[ 2 ] =
                    {
                        motor::math::vec4f_t ( 1.0f, 1.0f, 1.0f, 1.0f ),
                        motor::math::vec4f_t ( 1.0f, 0.5f, 0.5f, 1.0f )
                    } ;

                    size_t const sel = cur_ring == cur_ring_time ? 0 : 1 ;

                    array[ e ].col = ring_colors[ sel ] ;
                }


            }
        } ) ;

    } ) ;
}

//*****************************************************************************************
void_t scene_0::coreo_1( demos::iscene::on_graphics_data_in_t gd ) noexcept
{
    static float_t  angle_ = 0.0f ;
    angle_ += ( ( ( ( gd.dt ) ) ) * 2.0f * motor::math::constants<float_t>::pi() ) / 5.0f ;
    if ( angle_ > 4.0f * motor::math::constants<float_t>::pi() ) angle_ = 0.0f ;

    float_t s = 5.0f * std::sin( angle_ ) ;

    float_t const circle = 2.0f * motor::math::constants<float_t>::pi() ;

    _cubes_data.data_buffer().resize( this_t::num_objects() ).
        update< the_data >( [&] ( the_data * array, size_t const ne )
    {
        typedef motor::concurrent::range_1d<size_t> range_t ;
        auto const & range = range_t( 0, std::min( size_t( this_t::num_objects() ), this_t::num_objects() ) ) ;

        motor::concurrent::parallel_for<size_t>( range, [&] ( range_t const & r )
        {
            size_t const cubes_per_ring = _cubes_per_ring ;
            size_t const num_rings = this_t::num_objects() / cubes_per_ring ;

            for ( size_t e = r.begin(); e < r.end(); ++e )
            {
                // the ring this item (e) is on.
                size_t const cur_ring = ( e / cubes_per_ring ) + 0 ;
                size_t const nxt_ring = ( e / cubes_per_ring ) + 1 ;

                // the ring the time is on.
                size_t const cur_ring_time = ( gd.cur_time / _per_ring_milli ) % std::max( num_rings, size_t(1) ) ;

                float_t final_radius = _center_radius ;
                // radius computation
                {
                    // compute random radius
                    {
                        float_t const rand_rad = _center_rand_radius * _center_rand_radius_mult *
                            this_t::rand( e ) ;
                        final_radius += rand_rad ;
                    }

                    // add frequencies to radius
                    {
                        float_t radius = _center_radius + _inner_amp * motor::math::fn<float_t>::sin(
                            ( ( float_t( cur_ring ) / float_t( num_rings ) ) * _inner_freq + _inner_shift ) * circle ) ;

                        final_radius += radius ;
                    }

                    //
                    // compute lift up rings
                    //
                    if ( _ring_to_lift != size_t( -1 ) )
                    {
                        size_t const start = std::min(
                            size_t( std::max( int_t( _ring_to_lift ) - int_t( _ring_to_lift_range ), 0 ) ), this_t::num_rings() ) ;

                        size_t const end = std::min( _ring_to_lift + _ring_to_lift_range, this_t::num_rings() ) ; ;

                        if ( cur_ring >= start && cur_ring <= end )
                        {
                            float_t const range = float_t( _ring_to_lift_range )  ;
                            float_t const rad_add = 1.0f - motor::math::fn<float_t>::abs(
                                ( float_t( _ring_to_lift ) - float_t( cur_ring ) ) / range ) ;

                            final_radius += _ring_lift_radius * rad_add * 10.0f ;
                        }

                    }

                    {
                        final_radius = motor::math::fn<float_t>::clamp( final_radius, 10.0f, _max_center_radius ) ;
                    }
                }

                float_t angle = 0.0f ;
                // angle computation
                {
                    #if 0
                    // cur cube in ring in [0,1]
                    float_t e0 = ( ( e ) % cubes_per_ring ) / float_t( cubes_per_ring ) ;
                    angle = motor::math::fn<float_t>::mod( e0 * circle , circle ) ;

                    #else
                    float_t e0 = ( ( e ) % cubes_per_ring ) / float_t( cubes_per_ring ) ;
                    float_t const freq_add = float_t ( ( gd.cur_time >> 2 ) % 1000 ) / 1000.0f ;
                    float_t const freq_mult = float_t( cur_ring % 2 ) * 2.0f - 1.0f ;
                    angle = motor::math::fn<float_t>::mod( e0 * circle + ( freq_add * freq_mult ), circle ) ;
                    #endif
                }

                float_t directional_shift = 0.0f ;
                {
                    directional_shift =  _direction_shift_rand_mult * (this_t::rand( e + 17 )*2.0f-1.0f) ;
                }

                {
                    float_t t0 = float_t( cur_ring ) / float_t ( num_rings ) ;
                    float_t t1 = float_t( nxt_ring ) / float_t ( num_rings ) ;

                    {
                        t0 += _direction_offset ;
                        t1 += _direction_offset ;

                        if( _clamp_directionl_offset )
                        {
                            t0 = motor::math::fn<float_t>::mod( t0, 1.0 ) ;
                            t1 = motor::math::fn<float_t>::mod( t1, 1.0 ) ;
                        }
                    }

                    auto const v0 = this_t::pos_funk( t0 ) ;
                    auto const v1 = this_t::pos_funk( t1 )  ;

                    auto dir = ( v1 - v0 ).normalized() ;
                    auto const up = motor::math::vec3f_t( 0.0f, 1.0f, 0.0f ) ;
                    auto const ortho_ = ( up - ( dir * dir.dot( up ) ) ).normalized() ;

                    auto ortho = motor::math::quat4f_t( angle, dir ) * ortho_ ;
                    auto const right = dir.crossed( ortho ).normalized().negated() ;

                    float_t const radius = final_radius ;

                    auto const radius_vec = ortho * radius ;
                    auto const base = v0 + radius_vec + dir * directional_shift ;
                   

                    auto const pos = motor::math::vec4f_t( base, _cube_radius ) ;
                    array[ e ].frame_x = motor::math::vec4f_t( right, 1.0f ) ;
                    array[ e ].frame_y = motor::math::vec4f_t( ortho, 1.0f ) ;
                    array[ e ].frame_z = motor::math::vec4f_t( dir, 1.0f ) ;

                    array[ e ].pos = pos ;
                }

                // determine color per ring
                {
                    motor::math::vec4f_t ring_colors[ 2 ] =
                    {
                        motor::math::vec4f_t ( 1.0f, 1.0f, 1.0f, 1.0f ),
                        motor::math::vec4f_t ( 1.0f, 0.5f, 0.5f, 1.0f )
                    } ;

                    size_t const sel = cur_ring == cur_ring_time ? 0 : 1 ;

                    array[ e ].col = ring_colors[ sel ] ;
                }


            }
        } ) ;

    } ) ;
}