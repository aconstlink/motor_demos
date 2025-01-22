
#include "main.h"

using namespace demos ;

//******************************************************************************************************
void_t the_app::on_graphics( motor::application::app::graphics_data_in_t gd ) noexcept
{
    if ( _proceed_time ) cur_time += gd.milli_dt ;

    // global time 
    size_t const time = cur_time ;

    // change free camera
    if( cam_idx == 0 )
    {
        // change camera translation
        {
            camera[ 0 ].translate_by( helper::camera_controls::create_translation( gd.sec_dt, _cc ) ) ;
            camera[ 0 ].transform_by( helper::camera_controls::create_rotation( gd.sec_dt, _cc ) ) ;
        }
    }

    // change other camera
    {
        typedef motor::math::linear_bezier_spline< motor::math::vec3f_t > linearf_t ;
        typedef motor::math::cubic_hermit_spline< motor::math::vec3f_t > splinef_t ;

        typedef motor::math::keyframe_sequence< splinef_t > keyframe_sequencef_t ;
        typedef motor::math::keyframe_sequence< splinef_t > keyframe_sequencef_t ;

        keyframe_sequencef_t kf( motor::math::time_remap_funk_type::cycle ) ;
        kf.insert( keyframe_sequencef_t::keyframe_t( 0, motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ) ;
        kf.insert( keyframe_sequencef_t::keyframe_t( 1000, motor::math::vec3f_t( 1000.0f, 0.0f, 0.0f ) ) ) ;
        kf.insert( keyframe_sequencef_t::keyframe_t( 3000, motor::math::vec3f_t( 0.0f, 500.0f, 0.0f ) ) ) ;
        kf.insert( keyframe_sequencef_t::keyframe_t( 3500, motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ) ;

        keyframe_sequencef_t kf2( motor::math::time_remap_funk_type::cycle ) ;
        kf2.insert( keyframe_sequencef_t::keyframe_t( 0, motor::math::vec3f_t( 0.0f, 0.0f, -1000.0f ) ) ) ;
        kf2.insert( keyframe_sequencef_t::keyframe_t( 1000, motor::math::vec3f_t( 1000.0f, 0.0f, -1000.0f ) ) ) ;
        kf2.insert( keyframe_sequencef_t::keyframe_t( 3000, motor::math::vec3f_t( 0.0f, 500.0f, -1000.0f ) ) ) ;
        kf2.insert( keyframe_sequencef_t::keyframe_t( 5000, motor::math::vec3f_t( -1000.0f, -100.0f, -1000.0f ) ) ) ;
        kf2.insert( keyframe_sequencef_t::keyframe_t( 6500, motor::math::vec3f_t( 0.0f, 0.0f, -1000.0f ) ) ) ;

        motor::math::vec3f_t const up = motor::math::vec3f_t( 0.0f, 1.0f, 0.0f ).normalized() ;
        camera[ 1 ].look_at( kf2( time ), up , kf(time) ) ;

        // draw keyframe sequance as path
        if( cam_idx == 0 )
        {
            splinef_t spline = kf2.get_spline() ;

            // draw spline using lines
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
                pr.draw_circle( motor::math::mat3f_t::make_identity(), kf2( time ), 10.0f,
                    motor::math::vec4f_t( 1.0f, 1.0f, 1.0f, 1.0f ), motor::math::vec4f_t( 1.0f, 1.0f, 0.0f, 1.0f ), 20 ) ;
            }

            // draw control points
            {
                auto const points = spline.control_points() ;
                for( size_t i=0; i<points.size(); ++i )
                {
                    auto const pos = points[i] ;
                    pr.draw_circle( motor::math::mat3f_t::make_identity(), pos, 10.0f,
                        motor::math::vec4f_t( 1.0f, 0.0f, 0.0f, 1.0f ), motor::math::vec4f_t( 1.0f, 1.0f, 0.0f, 1.0f ), 20 ) ;
                }
            }
        }
    }

    // draw camera view volume
    if( cam_idx != 1 )
    {
        motor::gfx::generic_camera_ptr_t cam = &camera[1] ;

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
        else if( cam->is_orthographic() )
        {
            auto const nf = motor::math::vec2f_t( 50.0f, 1000.0f ) ;

            auto const cs = cam->get_dims().xy() * motor::math::vec2f_t(0.5f) ;
            {
                motor::math::vec3f_t const scale( cs.x(), cs.y(), nf.x() ) ;
                points[ 0 ] = motor::math::vec3f_t( -1.0f, -1.0f, 1.0f ) * scale ;
                points[ 1 ] = motor::math::vec3f_t( -1.0f, +1.0f, 1.0f ) * scale ;
                points[ 2 ] = motor::math::vec3f_t( +1.0f, +1.0f, 1.0f ) * scale ;
                points[ 3 ] = motor::math::vec3f_t( +1.0f, -1.0f, 1.0f ) * scale ;
            }

            {
                motor::math::vec3f_t const scale( cs.x(), cs.y(), nf.y() ) ;
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

        // front
        for( size_t i=0; i<4; ++i )
        {
            size_t const i0 = i + 0 ;
            size_t const i1 = (i + 1) % 4 ;
            pr.draw_line( points[ i0 ], points[ i1 ], motor::math::vec4f_t( 1.0f ) ) ;
        }

        // back
        for ( size_t i = 0; i < 4; ++i )
        {
            size_t const i0 = (i + 0) + 4;
            size_t const i1 = (( i + 1 ) % 4) + 4 ;
            pr.draw_line( points[ i0 ], points[ i1 ], motor::math::vec4f_t( 1.0f ) ) ;
        }


        // sides
        for ( size_t i = 0; i < 4; ++i )
        {
            size_t const i0 = ( i + 0 ) ;
            size_t const i1 = ( i + 4 ) % 8 ;
            pr.draw_line( points[ i0 ], points[ i1 ], motor::math::vec4f_t( 1.0f ) ) ;
        }
    }

    // 
    {
        typedef motor::math::cubic_hermit_spline< motor::math::vec3f_t > spline_t ;
        typedef motor::math::keyframe_sequence< spline_t > keyframe_sequence_t ;

        keyframe_sequence_t kf ;

        kf.insert( keyframe_sequence_t::keyframe_t( 0, motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ) ;
        kf.insert( keyframe_sequence_t::keyframe_t( 1000, motor::math::vec3f_t( 1.0f, 0.0f, 0.0f ) ) ) ;
        kf.insert( keyframe_sequence_t::keyframe_t( 4000, motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ) ;
        kf.insert( keyframe_sequence_t::keyframe_t( 2000, motor::math::vec3f_t( 0.0f, 0.0f, 1.0f ) ) ) ;
        kf.insert( keyframe_sequence_t::keyframe_t( 3000, motor::math::vec3f_t( 0.0f, 1.0f, 0.0f ) ) ) ;

        size_t ltime = time % kf.back().get_time() ;

        motor::math::vec4f_t color = motor::math::vec4f_t( kf( ltime ), 1.0f ) ;

        {
            motor::math::vec3f_t const start( -1000.0f, 0.0f, 0.0f ) ;
            size_t const ne = 100 ;
            float_t const step = 1.0f / ne ;
            for ( size_t i = 0; i < ne; ++i )
            {
                float_t const i_f = float_t( i ) / ne ;
                motor::math::vec3f_t pos = start + motor::math::vec3f_t( float_t( i ), 1.0f, 1.0f ) * motor::math::vec3f_t( 50.0f, 1.0f, 1.0f ) ;
                pr.draw_circle( motor::math::mat3f_t::make_identity(), pos, 30.0f, color, color, 20 ) ;
            }
        }

        {
            motor::math::vec3f_t const start( -300.0f, -100.0f, -100.0f ) ;
            size_t const ne = 100 ;
            float_t const step = 1.0f / ne ;
            for ( size_t i = 0; i < ne; ++i )
            {
                float_t const i_f = float_t( i ) * step ; 
                float_t const sin_y = motor::math::fn<float_t>::sin( i_f * 2.0f * motor::math::constants<float_t>::pi() ) ;
                motor::math::vec3f_t const off( 0.0f, 200.0f * sin_y, 0.0f ) ;
                motor::math::vec3f_t pos = start + off + motor::math::vec3f_t( float_t( i ), 1.0f, 1.0f ) * motor::math::vec3f_t( 10.0f, 1.0f, 1.0f ) ;
                pr.draw_circle( motor::math::mat3f_t::make_identity(), pos, 10.0f,
                    motor::math::vec4f_t( ( motor::math::vec4f_t( 1.0f ) - color ).xyz(), 1.0f ), color, 20 ) ;
            }
        }
    }

    // 
    {
        motor::math::vec3f_t const df( 100.0f ) ;

        typedef motor::math::cubic_hermit_spline< motor::math::vec3f_t > spline_t ;
        typedef motor::math::keyframe_sequence< spline_t > keyframe_sequence_t ;

        typedef motor::math::linear_bezier_spline< float_t > splinef_t ;
        typedef motor::math::keyframe_sequence< splinef_t > keyframe_sequencef_t ;

        keyframe_sequence_t kf ;

        motor::math::vec3f_t const p0 = motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) + df * motor::math::vec3f_t( -1.0f, -1.0f, 1.0f ) ;
        motor::math::vec3f_t const p1 = motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) + df * motor::math::vec3f_t( -1.0f, 1.0f, -1.0f ) ;
        motor::math::vec3f_t const p2 = motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) + df * motor::math::vec3f_t( 1.0f, 1.0f, 1.0f ) ;
        motor::math::vec3f_t const p3 = motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) + df * motor::math::vec3f_t( 1.0f, -1.0f, -1.0f ) ;

        kf.insert( keyframe_sequence_t::keyframe_t( 0, p0 ) ) ;
        kf.insert( keyframe_sequence_t::keyframe_t( 1000, p1 ) ) ;
        kf.insert( keyframe_sequence_t::keyframe_t( 2000, p2 ) ) ;
        kf.insert( keyframe_sequence_t::keyframe_t( 3000, p3 ) ) ;
        kf.insert( keyframe_sequence_t::keyframe_t( 4000, p0 ) ) ;


        keyframe_sequencef_t kf2 ;
        kf2.insert( keyframe_sequencef_t::keyframe_t( 0, 30.0f ) ) ;
        kf2.insert( keyframe_sequencef_t::keyframe_t( 1000, 50.0f ) ) ;
        kf2.insert( keyframe_sequencef_t::keyframe_t( 2000, 20.0f ) ) ;
        kf2.insert( keyframe_sequencef_t::keyframe_t( 3000, 60.0f ) ) ;
        kf2.insert( keyframe_sequencef_t::keyframe_t( 10000, 10.0f ) ) ;
        kf2.insert( keyframe_sequencef_t::keyframe_t( 11000, 30.0f ) ) ;


        size_t const ltime = time % kf.back().get_time() ;
        size_t const ltime2 = time % kf2.back().get_time() ;

        motor::math::vec4f_t color = motor::math::vec4f_t( 1.0f ) ;

        pr.draw_circle(
            ( camera[cam_idx].get_transformation() ).get_rotation_matrix(), kf( ltime ), kf2( ltime2 ), color, color, 10 ) ;
    }

    pr.set_view_proj( camera[cam_idx].mat_view(), camera[cam_idx].mat_proj() ) ;
    pr.prepare_for_rendering() ;
}