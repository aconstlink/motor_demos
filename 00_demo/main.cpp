
#include "main.h"

#include "scenes/scene_0.h"
#include "scenes/scene_1.h"




using namespace demos ;

//******************************************************************************************************
void_t the_app::on_init( void_t ) noexcept
{
    {
        motor::application::window_info_t wi ;
        wi.x = 100 ;
        wi.y = 100 ;
        wi.w = 800 ;
        wi.h = 1200 ;
        wi.gen = motor::application::graphics_generation::gen4_auto ;

        _twid = this_t::create_window( wi ) ;
        this_t::send_window_message( _twid, [&] ( motor::application::app::window_view & wnd )
        {
            wnd.send_message( motor::application::show_message( { true } ) ) ;
            wnd.send_message( motor::application::cursor_message_t( { true } ) ) ;
            wnd.send_message( motor::application::vsync_message_t( { true } ) ) ;
        } ) ;
    }
           
    #if 1
    {
        motor::application::window_info_t wi ;
        wi.x = 900 ;
        wi.y = 100 ;
        wi.w = 800 ;
        wi.h = 600 ;
        wi.gen = motor::application::graphics_generation::gen4_gl4 ;

        _dwid = this_t::create_window( wi ) ;
        this_t::send_window_message( _dwid, [&] ( motor::application::app::window_view & wnd )
        {
            wnd.send_message( motor::application::show_message( { true } ) ) ;
            wnd.send_message( motor::application::cursor_message_t( { true } ) ) ;
            wnd.send_message( motor::application::vsync_message_t( { true } ) ) ;
        } ) ;
    }
    #endif

    pr.init( "my_prim_render" ) ;

    // init scenes
    {
        // init manager
        {
            {
                using kfs_t = demos::scene_manager::keyframe_sequence_t ;
                kfs_t kf ;

                kf.insert( kfs_t::keyframe_t( 0, size_t(0) ) ) ;
                kf.insert( kfs_t::keyframe_t( 6500, size_t(1) ) ) ;
                kf.insert( kfs_t::keyframe_t( 12000, size_t(1) ) ) ;

                _scene_mgr.scene_selector = std::move( kf ) ;
            }
        }

        {
            _scenes_ar[ 0 ].cam_idx = 1 ;
        }

        {
            _scenes_ar[ 1 ].cam_idx = 2 ;
        }


        


    }

    // init cameras
    {
        for ( size_t i = 0; i < this_t::get_num_cams(); ++i )
        {
            _camera[ i ].cam.set_dims( 1.0f, 1.0f, 1.0f, 10000.0f ) ;
            _camera[ i ].cam.perspective_fov( motor::math::angle<float_t>::degree_to_radian( 45.0f ) ) ;
            _camera[ i ].cam.look_at( motor::math::vec3f_t( 0.0f, 0.0f, -500.0f ),
                motor::math::vec3f_t( 0.0f, 1.0f, 0.0f ), motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ;

            _camera[ i ].cam.set_sensor_dims( float_t( fb_dims.z() ), float_t( fb_dims.w() ) ) ;
            _camera[ i ].cam.perspective_fov() ;
        }

        // camera 1
        {
            auto & cam_data = _camera[ 1 ] ;

            cam_data.cam.look_at( motor::math::vec3f_t( 0.0f, 0.0f, -500.0f ),
                motor::math::vec3f_t( 0.0f, 1.0f, 0.0f ), motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ;

            {
                using kfs_t = demos::camera_data::keyframe_sequencef_t ;
                kfs_t kf_pos ;
                kf_pos.insert( kfs_t::keyframe_t( 0, motor::math::vec3f_t( 0.0f, 0.0f, -1000.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( 1000, motor::math::vec3f_t( 1000.0f, 0.0f, -1000.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( 3000, motor::math::vec3f_t( 0.0f, 500.0f, -1000.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( 5000, motor::math::vec3f_t( -1000.0f, -100.0f, -1000.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( 6500, motor::math::vec3f_t( 0.0f, 0.0f, -1000.0f ) ) ) ;
                cam_data.kf_pos = std::move( kf_pos ) ;
            }

            {
                using kfs_t = demos::camera_data::keyframe_sequencef_t ;

                kfs_t kf( motor::math::time_remap_funk_type::cycle ) ;
                kf.insert( kfs_t::keyframe_t( 0, motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 1000, motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 2000, motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ) ;
                cam_data.kf_lookat = std::move( kf ) ;
            }
        }

        // camera 2
        {
            auto & cam_data = _camera[ 2 ] ;
            cam_data.cam.look_at( motor::math::vec3f_t( 1000.0f, 100.0f, 500.0f ),
                motor::math::vec3f_t( 0.0f, 1.0f, 0.0f ), motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ;

            {
                auto const start = motor::math::vec3f_t( 1000.0f, 100.0f, 500.0f ) ;
                size_t const start_milli = 6500 ;

                using kfs_t = demos::camera_data::keyframe_sequencef_t ;
                kfs_t kf_pos ;
                kf_pos.insert( kfs_t::keyframe_t( start_milli, start ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( start_milli + 1000, start + motor::math::vec3f_t( 100.0f, 100.0f, -100.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( start_milli + 2000, start + motor::math::vec3f_t( -400.0f, -500.0f, 1000.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( start_milli + 3000, start + motor::math::vec3f_t( 0.0f, -100.0f, 0.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( start_milli + 4000, start ) ) ;
                cam_data.kf_pos = std::move( kf_pos ) ;
            }

            {
                using kfs_t = demos::camera_data::keyframe_sequencef_t ;

                kfs_t kf( motor::math::time_remap_funk_type::cycle ) ;
                kf.insert( kfs_t::keyframe_t( 0, motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 1000, motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 2000, motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ) ;
                cam_data.kf_lookat = std::move( kf ) ;
            }
        }
    }

    // post quad vertex/index buffer
    {
        struct vertex { motor::math::vec3f_t pos ; } ;

        auto vb = motor::graphics::vertex_buffer_t()
            .add_layout_element( motor::graphics::vertex_attribute::position, motor::graphics::type::tfloat, motor::graphics::type_struct::vec3 )
            .resize( 4 ).update<vertex>( [=] ( vertex * array, size_t const ne )
        {
            array[ 0 ].pos = motor::math::vec3f_t( -0.5f, -0.5f, 0.0f ) ;
            array[ 1 ].pos = motor::math::vec3f_t( -0.5f, +0.5f, 0.0f ) ;
            array[ 2 ].pos = motor::math::vec3f_t( +0.5f, +0.5f, 0.0f ) ;
            array[ 3 ].pos = motor::math::vec3f_t( +0.5f, -0.5f, 0.0f ) ;
        } );

        auto ib = motor::graphics::index_buffer_t().
            set_layout_element( motor::graphics::type::tuint ).resize( 6 ).
            update<uint_t>( [] ( uint_t * array, size_t const ne )
        {
            array[ 0 ] = 0 ;
            array[ 1 ] = 1 ;
            array[ 2 ] = 2 ;

            array[ 3 ] = 0 ;
            array[ 4 ] = 2 ;
            array[ 5 ] = 3 ;
        } ) ;

        _post_quad = motor::shared( motor::graphics::geometry_object_t( "post_quad",
            motor::graphics::primitive_type::triangles, std::move( vb ), std::move( ib ) ) ) ;
    }

    // post shader
    {
        motor::graphics::msl_object_t mslo( "color_to_screen" ) ;
        
        _db.load( motor::io::location_t( "shaders.post_process.color_to_screen.msl" ) ).wait_for_operation( 
            [&] ( char_cptr_t data, size_t const sib, motor::io::result const loading_res )
        {
            if( loading_res != motor::io::result::ok )
            {
                assert( false ) ;
            }

            mslo.add( motor::graphics::msl_api_type::msl_4_0, motor::string_t( data, sib ) ) ;
        } ) ;

        mslo.link_geometry( "post_quad" ) ;

        // variable sets
        {
            motor::graphics::variable_set_t vars ;
            {
                auto * var = vars.data_variable< motor::math::vec4f_t >( "u_color" ) ;
                var->set( motor::math::vec4f_t( 1.0f, 0.0f, 0.0f, 1.0f ) ) ;
            }

            {
                auto * var = vars.texture_variable( "tx_map" ) ;
                var->set( "the_scene.2" ) ;
            }

            mslo.add_variable_set( motor::shared( std::move( vars ), "a variable set" ) ) ;
        }

        _post_msl = motor::shared( std::move( mslo ) ) ;
    }

    {
        motor::graphics::state_object_t so = motor::graphics::state_object_t(
            "post_processing" ) ;

        {
            motor::graphics::render_state_sets_t rss ;
            rss.depth_s.do_change = true ;
            rss.depth_s.ss.do_activate = false ;

            rss.polygon_s.do_change = true ;
            rss.polygon_s.ss.do_activate = true ;
            rss.polygon_s.ss.ff = motor::graphics::front_face::clock_wise ;
            rss.polygon_s.ss.cm = motor::graphics::cull_mode::back ;
            
            rss.clear_s.do_change = false ;
            
            rss.view_s.do_change = false ;
            
            so.add_render_state_set( rss ) ;
        }

        _post_process_rs = std::move( so ) ;
    }

    {
        motor::graphics::state_object_t so = motor::graphics::state_object_t(
            "primitive renderer" ) ;

        {
            motor::graphics::render_state_sets_t rss ;
            rss.depth_s.do_change = true ;
            rss.depth_s.ss.do_activate = true ;
            rss.depth_s.ss.do_depth_write = true ;
            rss.polygon_s.do_change = true ;
            rss.polygon_s.ss.do_activate = true ;
            rss.polygon_s.ss.ff = motor::graphics::front_face::clock_wise ;
            rss.polygon_s.ss.cm = motor::graphics::cull_mode::back ;
            
            rss.clear_s.do_change = false ;
            rss.clear_s.ss.clear_color = motor::math::vec4f_t( 0.5f, 0.2f, 0.2f, 1.0f ) ;
            rss.clear_s.ss.do_activate = true ;
            rss.clear_s.ss.do_color_clear = true ;
            rss.clear_s.ss.do_depth_clear = true ;

            so.add_render_state_set( rss ) ;
        }

        _pr_rs = std::move( so ) ;
    }

    {
        motor::graphics::state_object_t so = motor::graphics::state_object_t(
            "root_render_states_final" ) ;

        {
            motor::graphics::render_state_sets_t rss ;
            rss.depth_s.do_change = true ;
            rss.depth_s.ss.do_activate = true ;
            rss.depth_s.ss.do_depth_write = true ;

            rss.polygon_s.do_change = true ;
            rss.polygon_s.ss.do_activate = true ;
            rss.polygon_s.ss.ff = motor::graphics::front_face::counter_clock_wise ;
            rss.polygon_s.ss.cm = motor::graphics::cull_mode::back ;
            rss.clear_s.do_change = true ;
            rss.clear_s.ss.clear_color = motor::math::vec4f_t( 0.5f, 0.5f, 0.5f, 1.0f ) ;
            rss.clear_s.ss.do_activate = true ;
            rss.clear_s.ss.do_color_clear = true ;
            rss.clear_s.ss.do_depth_clear = true ;
            rss.view_s.do_change = true ;
            rss.view_s.ss.do_activate = true ;
            rss.view_s.ss.vp = motor::math::vec4ui_t( 0, 0, fb_dims.z(), fb_dims.w() ) ;
            so.add_render_state_set( rss ) ;
        }

        _scene_final_rs = std::move( so ) ;
    }

    // framebuffer
    {
        pp_fb = motor::graphics::framebuffer_object_t( "the_scene" ) ;
        pp_fb.set_target( motor::graphics::color_target_type::rgba_uint_8, 3 )
            .set_target( motor::graphics::depth_stencil_target_type::depth32 )
            .resize( size_t(fb_dims.z()), size_t( fb_dims.w() ) ) ;
    }

    {
        {
            _scenes.emplace_back( motor::shared( demos::scene_0( "scene_0", 0, motor::math::time::to_milli( 1, 0, 0 ) ) ) ) ;
            // more here
        }

        for ( auto * s : _scenes )
        {
            s->on_init( _db ) ;
        }
    }
}

//******************************************************************************************************
void_t the_app::on_event( window_id_t const wid,
    motor::application::window_message_listener::state_vector_cref_t sv ) noexcept
{
    if ( sv.create_changed )
    {
        if ( _rwid == wid )
            motor::log::global_t::status( "[my_app] : render window created" ) ;
        else
            motor::log::global_t::status( "[my_app] : window created" ) ;


    }
    if ( sv.close_changed )
    {
        motor::log::global_t::status( "[my_app] : window closed" ) ;

        if ( _rwid == wid )
        {
            _rwid = size_t( -1 ) ;
        }
        else
        {
            this->close() ;
        }
    }
    if ( sv.resize_changed )
    {
        float_t const w = float_t( sv.resize_msg.w ) ;
        float_t const h = float_t( sv.resize_msg.h ) ;

        #if 0
        for ( size_t i = 0; i < 2; ++i )
        {
            camera[ i ].set_sensor_dims( w, h ) ;
            camera[ i ].perspective_fov() ;
        }
        #endif
    }
}

//******************************************************************************************************
void_t the_app::on_device( device_data_in_t dd ) noexcept
{
    helper::camera_controls::test_controls( dd.ascii, dd.mouse, _cc ) ;
            
    {
        using layout_t = motor::controls::types::ascii_keyboard_t ;
        using key_t = layout_t::ascii_key ;

        motor::controls::types::ascii_keyboard_t keyboard( dd.ascii ) ;

        if ( keyboard.get_state( key_t::k_1 ) ==
            motor::controls::components::key_state::released )
        {
            _cam_idx = 0 ;
        }
        else if ( keyboard.get_state( key_t::k_2 ) ==
            motor::controls::components::key_state::released )
        {
            _cam_idx = 1 ;
        }
        else if ( keyboard.get_state( key_t::k_3 ) ==
            motor::controls::components::key_state::released )
        {
            _cam_idx = ++_cam_idx % 2 ;
        }

        if ( keyboard.get_state( key_t::f3 ) ==
            motor::controls::components::key_state::released )
        {
            if( _rwid == size_t(-1) )
            {
                motor::application::window_info_t wi ;
                wi.x = 900 ;
                wi.y = 720 ;
                wi.w = 800 ;
                wi.h = 600 ;
                wi.gen = motor::application::graphics_generation::gen4_auto ;

                _rwid = this_t::create_window( wi );

                this_t::send_window_message( _rwid, [&] ( motor::application::app::window_view & wnd )
                {
                    wnd.send_message( motor::application::show_message( { true } ) ) ;
                    wnd.send_message( motor::application::cursor_message_t( { true } ) ) ;
                    wnd.send_message( motor::application::vsync_message_t( { true } ) ) ;
                } ) ;
            }
            else
            {
                this_t::send_window_message( _rwid, [&] ( motor::application::app::window_view & wnd )
                {
                    wnd.send_message( motor::application::close_message( { true } ) ) ;
                } ) ;
            }
        }
        else if ( keyboard.get_state( key_t::f4 ) == motor::controls::components::key_state::released &&
            _rwid != size_t( -1 ) )
        {
            this_t::send_window_message( _rwid, [&] ( motor::application::app::window_view & wnd )
            {
                wnd.send_message( motor::application::fullscreen_message(
                    {
                        motor::application::three_state::toggle,
                        motor::application::three_state::toggle
                    } ) ) ;
            } );
        }
    }
}

//******************************************************************************************************
void_t the_app::on_update( motor::application::app::update_data_in_t ud ) noexcept
{
    if ( _proceed_time ) cur_time += ud.milli_dt ;

    _final_cam_idx = _scenes_ar[ _scene_mgr.scene_selector( cur_time ) ].cam_idx ;
}

//******************************************************************************************************
void_t the_app::on_shutdown( void_t ) noexcept 
{
    motor::release( motor::move( _post_quad ) ) ;
    motor::release( motor::move( _post_msl ) ) ;
    motor::release( motor::move( _mon ) ) ;

    for( auto * s : _scenes )
    {
        s->on_release() ;
        motor::release( motor::move( s ) ) ;
    }
}

//******************************************************************************************************
int main( int argc, char ** argv )
{
    using namespace motor::core::types ;

    motor::application::carrier_mtr_t carrier = motor::platform::global_t::create_carrier(
        motor::shared( demos::the_app() ) ) ;

    auto const ret = carrier->exec() ;

    motor::memory::release_ptr( carrier ) ;

    motor::io::global::deinit() ;
    motor::concurrent::global::deinit() ;
    motor::log::global::deinit() ;
    motor::memory::global::dump_to_std() ;


    return ret ;
}
