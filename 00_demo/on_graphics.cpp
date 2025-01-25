
#include "main.h"

using namespace demos ;

//******************************************************************************************************
void_t the_app::on_graphics( motor::application::app::graphics_data_in_t gd ) noexcept
{
    // global time 
    size_t const time = cur_time ;

    // change free camera
    if( _cam_idx == 0 )
    {
        // change camera translation
        {
            _camera[ 0 ].cam.translate_by( helper::camera_controls::create_translation( gd.sec_dt, _cc ) ) ;
            _camera[ 0 ].cam.transform_by( helper::camera_controls::create_rotation( gd.sec_dt, _cc ) ) ;
        }
    }

    // move the camera on the path
    {
        for ( size_t c = 1; c < this_t::get_num_cams(); ++c )
        {
            auto & cam = _camera[ c ] ;

            motor::math::vec3f_t const up = motor::math::vec3f_t( 0.0f, 1.0f, 0.0f ).normalized() ;
            cam.cam.look_at( cam.kf_pos( time ), up, cam.kf_lookat( time ) ) ;
        }
    }

    // draw camera paths
    {
        using splinef_t = demos::camera_data::splinef_t ;

        for( size_t c=1; c<this_t::get_num_cams(); ++c )
        {
            auto & cam = _camera[c] ;

            splinef_t spline = _camera[c].kf_pos.get_spline() ;

            // draw spline using lines
            if( c == _cam_idx ) continue ;
            {
                size_t const num_steps = 300 ;
                for ( size_t i = 0; i < num_steps - 1; ++i )
                {
                    float_t const t0 = float_t( i + 0 ) / float_t ( num_steps - 1 ) ;
                    float_t const t1 = float_t( i + 1 ) / float_t ( num_steps - 1 ) ;

                    auto const v0 = spline( t0 ) ;
                    auto const v1 = spline( t1 ) ;

                    pr.draw_line( v0, v1, motor::math::vec4f_t( 1.0f, 1.0f, 1.0f, 1.0f ) ) ;
                }
            }

            // draw current position
            {
                pr.draw_circle( motor::math::mat3f_t::make_identity(), cam.kf_pos( time ), 10.0f,
                    motor::math::vec4f_t( 1.0f, 1.0f, 1.0f, 1.0f ), motor::math::vec4f_t( 1.0f, 1.0f, 0.0f, 1.0f ), 20 ) ;
            }

            // draw control points
            {
                auto const points = spline.control_points() ;
                for ( size_t i = 0; i < points.size(); ++i )
                {
                    auto const pos = points[ i ] ;
                    pr.draw_circle( motor::math::mat3f_t::make_identity(), pos, 10.0f,
                        motor::math::vec4f_t( 1.0f, 0.0f, 0.0f, 1.0f ), motor::math::vec4f_t( 1.0f, 1.0f, 0.0f, 1.0f ), 20 ) ;
                }
            }
        }

    }

    // draw camera view volume
    for( size_t c=1; c<this_t::get_num_cams(); ++c )
    {
        if( c == _cam_idx ) continue ;

        motor::gfx::generic_camera_ptr_t cam = &_camera[c].cam ;

        // 0-3 : front plane
        // 4-7 : back plane
        motor::math::vec3f_t points[8] ;

        if( cam->is_perspective() )
        {
            auto const frust = cam->get_frustum() ;

            //auto const nf = cam->get_near_far() ;
            auto const nf = motor::math::vec2f_t( 50.0f, 1000.0f ) ;

            auto const cs = cam->near_far_plane_half_dims( nf ) ;
            {
                motor::math::vec3f_t const scale( cs.x(), cs.y(), nf.x() ) ;
                points[ 0 ] = motor::math::vec3f_t( -1.0f, -1.0f, 1.0f ) * scale ;
                points[ 1 ] = motor::math::vec3f_t( -1.0f, +1.0f, 1.0f ) * scale ;
                points[ 2 ] = motor::math::vec3f_t( +1.0f, +1.0f, 1.0f ) * scale ;
                points[ 3 ] = motor::math::vec3f_t( +1.0f, -1.0f, 1.0f ) * scale ;
            }

            {
                motor::math::vec3f_t const scale( cs.z(), cs.w(), nf.y() ) ;
                points[ 4 ] = motor::math::vec3f_t( -1.0f, -1.0f, 1.0f ) * scale ;
                points[ 5 ] = motor::math::vec3f_t( -1.0f, +1.0f, 1.0f ) * scale ;
                points[ 6 ] = motor::math::vec3f_t( +1.0f, +1.0f, 1.0f ) * scale ;
                points[ 7 ] = motor::math::vec3f_t( +1.0f, -1.0f, 1.0f ) * scale ;
            }
        }

        for( size_t i=0; i<8; ++i )
        {
            points[i] = (cam->get_transformation().get_transformation() * motor::math::vec4f_t( points[i], 1.0f )).xyz() ;
        }

        motor::math::vec4f_t const color[] = 
        {
            motor::math::vec4f_t( 1.0f ),
            motor::math::vec4f_t( 1.0f, 0.0f, 0.0f, 1.0f )
        } ;

        size_t const cidx = c == _final_cam_idx ? 1 : 0 ;

        // front
        for( size_t i=0; i<4; ++i )
        {
            size_t const i0 = i + 0 ;
            size_t const i1 = (i + 1) % 4 ;
            pr.draw_line( points[ i0 ], points[ i1 ], color[cidx] ) ;
        }

        // back
        for ( size_t i = 0; i < 4; ++i )
        {
            size_t const i0 = (i + 0) + 4;
            size_t const i1 = (( i + 1 ) % 4) + 4 ;
            pr.draw_line( points[ i0 ], points[ i1 ], color[cidx] ) ;
        }


        // sides
        for ( size_t i = 0; i < 4; ++i )
        {
            size_t const i0 = ( i + 0 ) ;
            size_t const i1 = ( i + 4 ) % 8 ;
            pr.draw_line( points[ i0 ], points[ i1 ], color[cidx] ) ;
        }
    }

    pr.set_view_proj( _camera[_cam_idx].cam.mat_view(), _camera[_cam_idx].cam.mat_proj() ) ;
    pr.prepare_for_rendering() ;


    {
        for( auto * s : _scenes ) 
        {
            s->on_camera_final( &_camera[_final_cam_idx].cam ) ;
            s->on_camera_debug( &_camera[_cam_idx].cam ) ;
            
            {
                
                s->on_graphics( demos::iscene::on_graphics_data 
                    {
                        cur_time, gd.sec_dt, &pr
                    } ) ;
            }
        }
    }
    
}