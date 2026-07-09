
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

    // init quad
    {
        struct vertex
        {
            motor::math::vec2f_t pos;
        };

        auto vb = motor::graphics::vertex_buffer_t()
                      .add_layout_element( motor::graphics::vertex_attribute::position,
                          motor::graphics::type::tfloat, motor::graphics::type_struct::vec2 )
                      .resize( 4 )
                      .update< vertex >( [ = ]( vertex * array, size_t const ne )
        {
            array[ 0 ].pos = motor::math::vec2f_t( -0.5f, -0.5f );
            array[ 1 ].pos = motor::math::vec2f_t( -0.5f, +0.5f );
            array[ 2 ].pos = motor::math::vec2f_t( +0.5f, +0.5f );
            array[ 3 ].pos = motor::math::vec2f_t( +0.5f, -0.5f );
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

        _post_quad = motor::graphics::geometry_object_t( "post.quad",
            motor::graphics::primitive_type::triangles, std::move( vb ), std::move( ib ) );
    }

    {
        demos::scene_manager_t::init_data_t id;
        id.db = motor::share( _db );
        id.fb_dims = motor::math::vec4ui_t( 0, 0, 1920, 1080 );
        _sm.on_init( id );
    }
}

//******************************************************************************************************
void_t the_app::on_event( window_id_t const wid,
    motor::application::window_message_listener::state_vector_cref_t sv ) noexcept
{
    demos::scene_manager_t::event_data_t ed;
    ed.wid = wid;
    ed.wt = demos::window_type::invalid;
    ed.wt = wid == _twid ? demos::window_type::tool : ed.wt;
    ed.wt = wid == _rwid ? demos::window_type::production : ed.wt;
    ed.wt = wid == _dwid ? demos::window_type::debug : ed.wt;

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
        auto const x = int_t( sv.resize_msg.x );
        auto const y = int_t( sv.resize_msg.y );
        auto const w = uint_t( sv.resize_msg.w );
        auto const h = uint_t( sv.resize_msg.h );

        ed.window_size_changed = true;
        ed.window_pos = motor::math::vec2i_t( x, y );
        ed.window_dims = motor::math::vec2ui_t( w, h );
    }

    _sm.on_event( ed );
}

//******************************************************************************************************
void_t the_app::on_device( device_data_in_t dd ) noexcept
{
    {
        using layout_t = motor::controls::types::ascii_keyboard_t;
        using key_t = layout_t::ascii_key;

        motor::controls::types::ascii_keyboard_t keyboard( dd.ascii );

        if( keyboard.get_state( key_t::k_1 ) == motor::controls::components::key_state::released )
        {
        }
        else if( keyboard.get_state( key_t::k_2 ) ==
                 motor::controls::components::key_state::released )
        {
        }
        else if( keyboard.get_state( key_t::k_3 ) ==
                 motor::controls::components::key_state::released )
        {
        }

        if( keyboard.get_state( key_t::f2 ) == motor::controls::components::key_state::released )
        {
            _need_tool_view = !_need_tool_view;
        }
        else if( keyboard.get_state( key_t::f3 ) ==
                 motor::controls::components::key_state::released )
        {
            if( _rwid == size_t( -1 ) )
            {
                motor::application::window_info_t wi;
                wi.x = 900;
                wi.y = 720;
                wi.w = 800;
                wi.h = 600;
                wi.gen = motor::application::graphics_generation::gen4_auto;

                _rwid = this_t::create_window( wi );

                this_t::send_window_message( _rwid,
                    [ & ]( motor::application::app::window_view & wnd )
                {
                    wnd.send_message( motor::application::show_message( { true } ) );
                    wnd.send_message( motor::application::cursor_message_t( { true } ) );
                    wnd.send_message( motor::application::vsync_message_t( { true } ) );
                } );
            }
            else
            {
                this_t::send_window_message( _rwid,
                    [ & ]( motor::application::app::window_view & wnd )
                { wnd.send_message( motor::application::close_message( { true } ) ); } );
            }
        }
        else if( keyboard.get_state( key_t::f4 ) ==
                     motor::controls::components::key_state::released &&
                 _rwid != size_t( -1 ) )
        {
            this_t::send_window_message( _rwid, [ & ]( motor::application::app::window_view & wnd )
            {
                wnd.send_message( motor::application::fullscreen_message(
                    { motor::application::three_state::toggle,
                        motor::application::three_state::toggle } ) );
            } );
        }

        else if( keyboard.get_state( key_t::c ) ==
                 motor::controls::components::key_state::released )
        {
        }
        else if( keyboard.get_state( key_t::p ) ==
                 motor::controls::components::key_state::released )
        {
        }

        else if( keyboard.get_state( key_t::space ) ==
                 motor::controls::components::key_state::released )
        {
            _space_bar_pressed = true;
        }
    }
}

//******************************************************************************************************
void_t the_app::on_update( motor::application::app::update_data_in_t ud ) noexcept
{
    if( _proceed_time ) _cur_time += ud.milli_dt;
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
void_t the_app::on_graphics( motor::application::app::graphics_data_in_t gd ) noexcept {}

//******************************************************************************************************
void_t the_app::on_render( this_t::window_id_t const wid, motor::graphics::gen4::frontend_ptr_t fe,
    motor::application::app::render_data_in_t rd ) noexcept
{
    if( rd.first_frame )
    {
        // init debug window rendering objects
        if( wid == _dwid )
        {
            pr.configure( fe );
            fe->configure< motor::graphics::state_object_t >( &_pr_rs );
        }

        fe->configure<motor::graphics::geometry_object_t>( &_post_quad ) ;
    }

    /////////////////////////////////////////////////////////////////////
    // handle render scene states
    /////////////////////////////////////////////////////////////////////
    {
        demos::scene_manager_t::render_data_t urd;
        urd.fe = fe;
        urd.wid = wid;
        urd.wt = demos::window_type::invalid;

        urd.wt = wid == _twid ? demos::window_type::tool : urd.wt;
        urd.wt = wid == _dwid ? demos::window_type::debug : urd.wt;
        urd.wt = wid == _rwid ? demos::window_type::production : urd.wt;

        urd.first_frame = rd.first_frame;
        urd.last_frame = rd.last_frame;

#if 0
        urd.fb_0 = &_pp_fb0;
        urd.fb_1 = &_pp_fb1;
#endif
        _sm.on_render( urd );
    }

    if( wid == _rwid && rd.last_frame ) _rwid = size_t( -1 );
}

//******************************************************************************************************
void_t the_app::on_shutdown( void_t ) noexcept
{
    {
        _sm.on_shutdown();
    }

    motor::release( motor::move( _mon ) );
    motor::release( motor::move( _db ) );
}