
#include "the_app.h"

#include <motor/tool/imgui/player_controller.h>

using namespace demos;

//******************************************************************************************************
the_app::the_app( demos::scene_manager_rref_t sm ) noexcept : _sm( std::move( sm ) ) {}

//******************************************************************************************************
the_app::the_app( this_rref_t rhv ) noexcept : _sm( std::move( rhv._sm ) )
{
    _gbuffer_selection = rhv._gbuffer_selection;
    _max_time_milli = rhv._max_time_milli;
    _mon = motor::move( rhv._mon );
    _db = motor::move( rhv._db );
}

//******************************************************************************************************
the_app::~the_app( void_t ) noexcept
{
    motor::release( motor::move( _db ) );
    motor::release( motor::move( _mon ) );
}

//******************************************************************************************************
void_t the_app::on_init( void_t ) noexcept
{
    _db =
        motor::shared( motor::io::database( motor::io::path_t( DATAPATH ), "./working", "data" ) );

    if( this_t::is_tool_mode() )
    {
        // tool window
        {
            motor::application::window_info_t wi;
            wi.x = 100;
            wi.y = 100;
            wi.w = 800;
            wi.h = 1200;
            wi.gen = motor::application::graphics_generation::gen4_auto;

            _twid = this_t::create_window( wi );
            this_t::send_window_message( _twid, [ & ]( motor::application::app::window_view & wnd )
            {
                wnd.send_message( motor::application::show_message( { true } ) );
                wnd.send_message( motor::application::cursor_message_t( { true } ) );
                wnd.send_message( motor::application::vsync_message_t( { true } ) );
            } );
        }

        // debug window
        {
            motor::application::window_info_t wi;
            wi.x = 900;
            wi.y = 100;
            wi.w = 800;
            wi.h = 600;
            wi.gen = motor::application::graphics_generation::gen4_gl4;

            _dwid = this_t::create_window( wi );
            this_t::send_window_message( _dwid, [ & ]( motor::application::app::window_view & wnd )
            {
                wnd.send_message( motor::application::show_message( { true } ) );
                wnd.send_message( motor::application::cursor_message_t( { true } ) );
                wnd.send_message( motor::application::vsync_message_t( { true } ) );
            } );
        }

        pr.init( "prims_for_debug" );
    }
    else // production window
    {
        motor::application::window_info_t wi;
        wi.x = 100;
        wi.y = 100;
        wi.w = 1280;
        wi.h = 960;
        wi.gen = motor::application::graphics_generation::gen4_auto;

        _rwid = this_t::create_window( wi );
        this_t::send_window_message( _rwid, [ & ]( motor::application::app::window_view & wnd )
        {
            wnd.send_message( motor::application::show_message( { true } ) );
            wnd.send_message( motor::application::cursor_message_t( { true } ) );
            wnd.send_message( motor::application::vsync_message_t( { true } ) );
        } );
    }

#if 0
    {
        _camera.set_dims( 1.0f, 1.0f, 1.0f, 10000.0f ) ;
        _camera.perspective_fov( motor::math::angle<float_t>::degree_to_radian( 45.0f ) ) ;
        _camera.set_sensor_dims( float_t( 1920 ), float_t( 1080 ) ) ;
        _camera.look_at( motor::math::vec3f_t( 0.0f, 0.0f, -500.0f ),
            motor::math::vec3f_t( 0.0f, 1.0f, 0.0f ), motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ;
    }
#endif
    // post quad vertex/index buffer
    {
        struct vertex
        {
            motor::math::vec3f_t pos;
        };

        auto vb = motor::graphics::vertex_buffer_t()
                      .add_layout_element( motor::graphics::vertex_attribute::position,
                          motor::graphics::type::tfloat, motor::graphics::type_struct::vec3 )
                      .resize( 4 )
                      .update< vertex >( [ = ]( vertex * array, size_t const ne )
        {
            array[ 0 ].pos = motor::math::vec3f_t( -0.5f, -0.5f, 0.0f );
            array[ 1 ].pos = motor::math::vec3f_t( -0.5f, +0.5f, 0.0f );
            array[ 2 ].pos = motor::math::vec3f_t( +0.5f, +0.5f, 0.0f );
            array[ 3 ].pos = motor::math::vec3f_t( +0.5f, -0.5f, 0.0f );
        } );

        auto ib = motor::graphics::index_buffer_t()
                      .set_layout_element( motor::graphics::type::tuint )
                      .resize( 6 )
                      .update< uint_t >( []( uint_t * array, size_t const ne )
        {
            array[ 0 ] = 0;
            array[ 1 ] = 1;
            array[ 2 ] = 2;

            array[ 3 ] = 0;
            array[ 4 ] = 2;
            array[ 5 ] = 3;
        } );

        _post_quad = motor::shared(
            motor::graphics::geometry_object_t( "post_quad",
                motor::graphics::primitive_type::triangles, std::move( vb ), std::move( ib ) ),
            "post quad" );
    }

    // post shader
    {
        motor::graphics::msl_object_t mslo( "color_to_screen" );

        _db->load( motor::io::location_t( "shaders.post_process.color_to_screen.msl" ) )
            .wait_for_operation(
                [ & ]( char_cptr_t data, size_t const sib, motor::io::result const loading_res )
        {
            if( loading_res != motor::io::result::ok )
            {
                assert( false );
            }

            mslo.add( motor::graphics::msl_api_type::msl_4_0, motor::string_t( data, sib ) );
        } );

        mslo.link_geometry( "post_quad" );

        // variable sets
        {
            motor::graphics::variable_set_t vars;
            {
                auto * var = vars.data_variable< motor::math::vec4f_t >( "u_color" );
                var->set( motor::math::vec4f_t( 1.0f, 0.0f, 0.0f, 1.0f ) );
            }

            {
                auto * var = vars.texture_variable( "tx_map" );
                var->set( "the_scene_0.2" );
            }

            mslo.add_variable_set( motor::shared( std::move( vars ), "a variable set" ) );
        }

        _post_msl = motor::shared( std::move( mslo ), "post msl" );
    }

    // post shader
    {
        motor::graphics::msl_object_t mslo( "xfade_to_screen" );

        _db->load( motor::io::location_t( "shaders.post_process.xfade_to_screen.msl" ) )
            .wait_for_operation(
                [ & ]( char_cptr_t data, size_t const sib, motor::io::result const loading_res )
        {
            if( loading_res != motor::io::result::ok )
            {
                assert( false );
            }

            mslo.add( motor::graphics::msl_api_type::msl_4_0, motor::string_t( data, sib ) );
        } );

        mslo.link_geometry( "post_quad" );

        // variable sets
        {
            motor::graphics::variable_set_t vars;
            {
                auto * var = vars.data_variable< motor::math::vec4f_t >( "u_color" );
                var->set( motor::math::vec4f_t( 1.0f, 0.0f, 0.0f, 1.0f ) );
            }

            {
                auto * var = vars.texture_variable( "tx_0_map" );
                var->set( "the_scene_0.2" );
            }

            {
                auto * var = vars.texture_variable( "tx_1_map" );
                var->set( "the_scene_1.2" );
            }

            {
                auto * var = vars.data_variable< float_t >( "u_overlap" );
                var->set( 0.5f );
            }

            mslo.add_variable_set( motor::shared( std::move( vars ), "a variable set" ) );
        }

        _post_xfade_msl = motor::shared( std::move( mslo ), "post xfade" );
    }

    {
        motor::graphics::state_object_t so = motor::graphics::state_object_t( "post_processing" );

        {
            motor::graphics::render_state_sets_t rss;
            rss.depth_s.do_change = true;
            rss.depth_s.ss.do_activate = false;

            rss.polygon_s.do_change = true;
            rss.polygon_s.ss.do_activate = true;
            rss.polygon_s.ss.ff = motor::graphics::front_face::clock_wise;
            rss.polygon_s.ss.cm = motor::graphics::cull_mode::back;

            rss.clear_s.do_change = false;

            rss.view_s.do_change = false;

            so.add_render_state_set( rss );
        }

        _post_process_rs = std::move( so );
    }

    {
        motor::graphics::state_object_t so =
            motor::graphics::state_object_t( "primitive renderer" );

        {
            motor::graphics::render_state_sets_t rss;
            rss.depth_s.do_change = true;
            rss.depth_s.ss.do_activate = true;
            rss.depth_s.ss.do_depth_write = false;
            rss.polygon_s.do_change = true;
            rss.polygon_s.ss.do_activate = true;
            rss.polygon_s.ss.ff = motor::graphics::front_face::clock_wise;
            rss.polygon_s.ss.cm = motor::graphics::cull_mode::back;

            rss.clear_s.do_change = false;
            rss.clear_s.ss.clear_color = motor::math::vec4f_t( 0.5f, 0.2f, 0.2f, 1.0f );
            rss.clear_s.ss.do_activate = false;
            rss.clear_s.ss.do_color_clear = true;
            rss.clear_s.ss.do_depth_clear = true;

            so.add_render_state_set( rss );
        }

        _pr_rs = std::move( so );
    }

    {
        motor::graphics::state_object_t so =
            motor::graphics::state_object_t( "debug scene root render states" );

        {
            motor::graphics::render_state_sets_t rss;
            rss.depth_s.do_change = true;
            rss.depth_s.ss.do_activate = true;
            rss.depth_s.ss.do_depth_write = true;
            rss.polygon_s.do_change = true;
            rss.polygon_s.ss.do_activate = true;
            rss.polygon_s.ss.ff = motor::graphics::front_face::counter_clock_wise;
            rss.polygon_s.ss.cm = motor::graphics::cull_mode::back;
            rss.polygon_s.ss.fm = motor::graphics::fill_mode::fill;

            rss.clear_s.do_change = true;
            rss.clear_s.ss.clear_color = motor::math::vec4f_t( 0.5f, 0.2f, 0.2f, 1.0f );
            rss.clear_s.ss.do_activate = true;
            rss.clear_s.ss.do_color_clear = true;
            rss.clear_s.ss.do_depth_clear = true;

            so.add_render_state_set( rss );
        }

        _dv_rs = std::move( so );
    }

    {
        motor::graphics::state_object_t so =
            motor::graphics::state_object_t( "root_render_states_final" );

        {
            motor::graphics::render_state_sets_t rss;
            rss.depth_s.do_change = true;
            rss.depth_s.ss.do_activate = true;
            rss.depth_s.ss.do_depth_write = true;

            rss.polygon_s.do_change = true;
            rss.polygon_s.ss.do_activate = true;
            rss.polygon_s.ss.ff = motor::graphics::front_face::counter_clock_wise;
            rss.polygon_s.ss.cm = motor::graphics::cull_mode::back;
            rss.clear_s.do_change = true;
            rss.clear_s.ss.clear_color = motor::math::vec4f_t( 0.5f, 0.5f, 0.5f, 1.0f );
            rss.clear_s.ss.do_activate = true;
            rss.clear_s.ss.do_color_clear = true;
            rss.clear_s.ss.do_depth_clear = true;
            rss.view_s.do_change = true;
            rss.view_s.ss.do_activate = true;
            rss.view_s.ss.vp = motor::math::vec4ui_t( 0, 0, fb_dims.z(), fb_dims.w() );
            so.add_render_state_set( rss );
        }

        _scene_final_rs = std::move( so );
    }

    // framebuffer
    {
        {
            pp_fb0 = motor::graphics::framebuffer_object_t( "the_scene_0" );
            pp_fb0.set_target( motor::graphics::color_target_type::rgba_uint_8, 3 )
                .set_target( motor::graphics::depth_stencil_target_type::depth32 )
                .resize( size_t( fb_dims.z() ), size_t( fb_dims.w() ) );
        }
        {
            pp_fb1 = motor::graphics::framebuffer_object_t( "the_scene_1" );
            pp_fb1.set_target( motor::graphics::color_target_type::rgba_uint_8, 3 )
                .set_target( motor::graphics::depth_stencil_target_type::depth32 )
                .resize( size_t( fb_dims.z() ), size_t( fb_dims.w() ) );
        }
    }
    // init scene manager
    {
    }
}

//******************************************************************************************************
void_t the_app::on_event( window_id_t const wid,
    motor::application::window_message_listener::state_vector_cref_t sv ) noexcept
{
    if( sv.create_changed )
    {
        if( _rwid == wid )
            motor::log::global_t::status( "[my_app] : production window created" );
        else
            motor::log::global_t::status( "[my_app] : window created" );
    }

    if( sv.close_changed )
    {
        motor::log::global_t::status( "[my_app] : window closed" );

        if( _rwid == wid )
        {
            if( !this_t::is_tool_mode() ) this->close();
        }
        else
        {
            this->close();
        }
    }

    if( sv.resize_changed )
    {
        auto const w = uint_t( sv.resize_msg.w );
        auto const h = uint_t( sv.resize_msg.h );
    }
}

//******************************************************************************************************
void_t the_app::on_device( device_data_in_t dd ) noexcept {}

//******************************************************************************************************
void_t the_app::on_update( motor::application::app::update_data_in_t ud ) noexcept
{
    if ( _proceed_time ) _cur_time += ud.milli_dt ;
    _cont_time = ( _cont_time + ud.milli_dt ) % motor::math::time_ms_t( 10000 );    

    {
        demos::scene_manager_t::update_data sud;
        sud.demo_time = _cur_time;
        sud.cont_time = _cont_time;
        sud.db = _db;
        _cur_time = _sm.on_scene_update( sud );
    }
}

//******************************************************************************************************
void_t the_app::on_shutdown( void_t ) noexcept
{
    {
        _sm.on_shutdown();
    }

    motor::release( motor::move( _post_quad ) );
    motor::release( motor::move( _post_msl ) );
    motor::release( motor::move( _post_xfade_msl ) );
    motor::release( motor::move( _mon ) );
    motor::release( motor::move( _db ) );
}