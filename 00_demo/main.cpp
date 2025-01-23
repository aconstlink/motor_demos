
#include "main.h"

using namespace demos ;


//******************************************************************************************************
void_t the_app::on_init( void_t ) noexcept
{
    {
        motor::application::window_info_t wi ;
        wi.x = 100 ;
        wi.y = 100 ;
        wi.w = 800 ;
        wi.h = 600 ;
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


    for( size_t i=0; i<2; ++i )
    {
        camera[i].set_dims( 1.0f, 1.0f, 1.0f, 10000.0f ) ;
        camera[i].perspective_fov( motor::math::angle<float_t>::degree_to_radian( 45.0f ) ) ;
        camera[i].look_at( motor::math::vec3f_t( 0.0f, 0.0f, -500.0f ),
            motor::math::vec3f_t( 0.0f, 1.0f, 0.0f ), motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ;

        camera[ i ].set_sensor_dims( float_t(fb_dims.z()), float_t(fb_dims.w()) ) ;
        camera[ i ].perspective_fov() ;
    }

    {
        motor::graphics::state_object_t so = motor::graphics::state_object_t(
            "root_render_states" ) ;

        {
            motor::graphics::render_state_sets_t rss ;
            rss.depth_s.do_change = true ;
            rss.depth_s.ss.do_activate = true ;
            rss.depth_s.ss.do_depth_write = true ;
            rss.polygon_s.do_change = true ;
            rss.polygon_s.ss.do_activate = true ;
            rss.polygon_s.ss.ff = motor::graphics::front_face::clock_wise ;
            rss.polygon_s.ss.cm = motor::graphics::cull_mode::back ;
            rss.clear_s.do_change = true ;
            rss.clear_s.ss.clear_color = motor::math::vec4f_t( 0.5f, 0.2f, 0.2f, 1.0f ) ;
            rss.clear_s.ss.do_activate = true ;
            rss.clear_s.ss.do_color_clear = true ;
            rss.clear_s.ss.do_depth_clear = true ;
            rss.view_s.do_change = true ;
            rss.view_s.ss.do_activate = true ;
            rss.view_s.ss.vp = motor::math::vec4ui_t( 0, 0, fb_dims.z(), fb_dims.w() ) ;
            so.add_render_state_set( rss ) ;
        }

        rs = std::move( so ) ;
    }

    {
        motor::graphics::state_object_t so = motor::graphics::state_object_t(
            "debug render state" ) ;

        {
            motor::graphics::render_state_sets_t rss ;
            rss.depth_s.do_change = true ;
            rss.depth_s.ss.do_activate = true ;
            rss.depth_s.ss.do_depth_write = true ;
            rss.polygon_s.do_change = true ;
            rss.polygon_s.ss.do_activate = true ;
            rss.polygon_s.ss.ff = motor::graphics::front_face::clock_wise ;
            rss.polygon_s.ss.cm = motor::graphics::cull_mode::back ;
            rss.clear_s.do_change = true ;
            rss.clear_s.ss.clear_color = motor::math::vec4f_t( 0.5f, 0.2f, 0.2f, 1.0f ) ;
            rss.clear_s.ss.do_activate = true ;
            rss.clear_s.ss.do_color_clear = true ;
            rss.clear_s.ss.do_depth_clear = true ;
            

            so.add_render_state_set( rss ) ;
        }

        _pr_rs = std::move( so ) ;
    }

    // framebuffer
    {
        pp_fb = motor::graphics::framebuffer_object_t( "the_scene" ) ;
        pp_fb.set_target( motor::graphics::color_target_type::rgba_uint_8, 3 )
            .set_target( motor::graphics::depth_stencil_target_type::depth32 )
            .resize( size_t(fb_dims.z()), size_t( fb_dims.w() ) ) ;
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
            cam_idx = 0 ;
        }
        else if ( keyboard.get_state( key_t::k_2 ) ==
            motor::controls::components::key_state::released )
        {
            cam_idx = 1 ;
        }
        else if ( keyboard.get_state( key_t::k_3 ) ==
            motor::controls::components::key_state::released )
        {
            cam_idx = ++cam_idx % 2 ;
        }

        if ( keyboard.get_state( key_t::f3 ) ==
            motor::controls::components::key_state::released )
        {
            if( _rwid == size_t(-1) )
            {
                motor::application::window_info_t wi ;
                wi.x = 900 ;
                wi.y = 500 ;
                wi.w = 800 ;
                wi.h = 600 ;
                wi.gen = motor::application::graphics_generation::gen4_auto ;

                _rwid = this_t::create_window( wi );

                this_t::send_window_message( _rwid, [&] ( motor::application::app::window_view & wnd )
                {
                    wnd.send_message( motor::application::show_message( { true } ) ) ;
                    wnd.send_message( motor::application::cursor_message_t( { false } ) ) ;
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
void_t the_app::on_update( motor::application::app::update_data_in_t ) noexcept
{
}

//******************************************************************************************************
int main( int argc, char ** argv )
{
    using namespace motor::core::types ;

    motor::application::carrier_mtr_t carrier = motor::platform::global_t::create_carrier(
        motor::shared( demos::the_app() ) ) ;

    auto const ret = carrier->exec() ;

    motor::memory::release_ptr( carrier ) ;

    motor::concurrent::global::deinit() ;
    motor::log::global::deinit() ;
    motor::memory::global::dump_to_std() ;


    return ret ;
}
