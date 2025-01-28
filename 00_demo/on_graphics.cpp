
#include "main.h"

using namespace demos ;

//******************************************************************************************************
void_t the_app::on_graphics( motor::application::app::graphics_data_in_t gd ) noexcept
{
    static size_t frame_count = 0 ;
    ++frame_count ;

    // global time 
    size_t const time = cur_time ;

    // change free camera
    if( _use_free_camera )
    {
        // change camera translation
        {
            _camera.translate_by( helper::camera_controls::create_translation( gd.sec_dt, _cc ) ) ;
            _camera.transform_by( helper::camera_controls::create_rotation( gd.sec_dt, _cc ) ) ;
        }
    }

    
    for( auto * s : _scenes )
    {
        auto & camera_manager = s->camera_manager() ;

        camera_manager.for_each_camera( [&]( size_t const idx, demos::camera_data & cd )
        {
            if ( idx == camera_manager.get_debug_camera_idx() && !_use_free_camera ) return ;
            auto & cam = cd ;

            // draw camera paths
            {
                motor::math::vec4f_t const color[] =
                {
                    motor::math::vec4f_t( 1.0f, 1.0f, 1.0f, 0.5f ),
                    motor::math::vec4f_t( 0.5f, 0.0f, 0.0f, 1.0f )
                } ;

                size_t const cidx = idx == s->camera_manager().get_final_camera_idx() ? 1 : 0 ;

                using splinef_t = demos::camera_data::splinef_t ;
                splinef_t spline = cam.kf_pos.get_spline() ;

                {
                    size_t const num_steps = 300 ;
                    pr.draw_lines( num_steps, [&]( size_t const i )
                    {
                        float_t const t0 = float_t( i + 0 ) / float_t ( num_steps - 1 ) ;
                        float_t const t1 = float_t( i + 1 ) / float_t ( num_steps - 1 ) ;

                        auto const v0 = spline( t0 ) ;
                        auto const v1 = spline( t1 ) ;

                        return motor::gfx::line_render_3d::draw_line_data { v0, v1, color[ cidx ] } ;
                    } ) ;
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

            // draw frustum
            {
                motor::gfx::generic_camera_ptr_t gcam = &cd.cam ;

                // 0-3 : front plane
                // 4-7 : back plane
                motor::math::vec3f_t points[ 8 ] ;

                if ( gcam->is_perspective() )
                {
                    auto const frust = gcam->get_frustum() ;

                    //auto const nf = cam->get_near_far() ;
                    auto const nf = motor::math::vec2f_t( 50.0f, 1000.0f ) ;

                    auto const cs = gcam->near_far_plane_half_dims( nf ) ;
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

                for ( size_t i = 0; i < 8; ++i )
                {
                    points[ i ] = ( gcam->get_transformation().get_transformation() * motor::math::vec4f_t( points[ i ], 1.0f ) ).xyz() ;
                }

                motor::math::vec4f_t const color[] =
                {
                    motor::math::vec4f_t( 1.0f, 1.0f, 1.0f, 0.5f ),
                    motor::math::vec4f_t( 1.0f, 0.0f, 0.0f, 1.0f )
                } ;

                size_t const cidx = idx == s->camera_manager().get_final_camera_idx() ? 1 : 0 ;

                // front
                for ( size_t i = 0; i < 4; ++i )
                {
                    size_t const i0 = i + 0 ;
                    size_t const i1 = ( i + 1 ) % 4 ;
                    pr.draw_line( points[ i0 ], points[ i1 ], color[ cidx ] ) ;
                }

                // back
                for ( size_t i = 0; i < 4; ++i )
                {
                    size_t const i0 = ( i + 0 ) + 4;
                    size_t const i1 = ( ( i + 1 ) % 4 ) + 4 ;
                    pr.draw_line( points[ i0 ], points[ i1 ], color[ cidx ] ) ;
                }

                // sides
                for ( size_t i = 0; i < 4; ++i )
                {
                    size_t const i0 = ( i + 0 ) ;
                    size_t const i1 = ( i + 4 ) % 8 ;
                    pr.draw_line( points[ i0 ], points[ i1 ], color[ cidx ] ) ;
                }
            }
        } ) ;
    }

    // set camera data to primitive renderer and do preparation
    {
        motor::gfx::generic_camera_ptr_t cam = _use_free_camera ? &_camera : 
            this_t::get_current_scene()->camera_manager().borrow_debug_camera() ; 

        pr.set_view_proj( cam->mat_view(), cam->mat_proj() ) ;
        pr.prepare_for_rendering() ;
    }

    // scene on_graphcis
    {
        for( auto * s : _scenes ) 
        {
            s->on_graphics( demos::iscene::on_graphics_data
            {
                cur_time, gd.sec_dt, &pr,
                _use_free_camera ? &_camera : nullptr
            } ) ;
        }
    }
    
}