
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
        
        #if 1
        camera[ 1 ].look_at( kf2( time ), up , motor::math::vec3f_t() ) ;
        #else
        camera[ 1 ].look_at( kf2( time ), up , kf(time) ) ;
        #endif

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

    pr.set_view_proj( camera[cam_idx].mat_view(), camera[cam_idx].mat_proj() ) ;
    pr.prepare_for_rendering() ;


    {
        _dummy_render_msl->for_each( [&]( size_t const i, motor::graphics::variable_set_mtr_t vs ) 
        {
            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "view" ) ;
                var->set( camera[_final_cam_idx].mat_view() ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "proj" ) ;
                var->set( camera[ _final_cam_idx ].mat_proj() ) ;
            }
        } ) ;
    }

    {
        _dummy_debug_msl->for_each( [&] ( size_t const i, motor::graphics::variable_set_mtr_t vs )
        {
            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "view" ) ;
                var->set( camera[ cam_idx ].mat_view() ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "proj" ) ;
                var->set( camera[ cam_idx ].mat_proj() ) ;
            }
        } ) ;
    }
}