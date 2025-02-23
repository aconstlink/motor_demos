
#include "main.h"

#include "scenes/scene_0.h"
#include "scenes/scene_1.h"


using namespace demos ;

//******************************************************************************************************
the_app::the_app( void_t ) noexcept
{
}

//******************************************************************************************************
the_app::the_app( this_rref_t rhv ) noexcept 
{
    _gbuffer_selection = rhv._gbuffer_selection ;
    _max_time_milli = rhv._max_time_milli ;
    _mon = motor::move( rhv._mon ) ;
    _db = motor::move( rhv._db ) ;
}

//******************************************************************************************************
the_app::~the_app( void_t ) noexcept 
{
}

//******************************************************************************************************
bool_t the_app::get_current_scene_data( this_t::scene_data & ret ) const noexcept 
{
    if( _cur_scene_idx == size_t(-1)  ) return false ;
    ret = _scenes[_cur_scene_idx] ;
    return true ;
}

//******************************************************************************************************
bool_t the_app::access_current_scene_data( std::function< void_t (scene_data & sd ) > funk ) noexcept 
{
    if( _cur_scene_idx == size_t(-1)  ) return false ;
    funk( _scenes[ _cur_scene_idx ] ) ;
    return true ;
}

//******************************************************************************************************
bool_t the_app::call_for_current_scene( std::function< void_t ( demos::iscene_mtr_t ) > funk ) noexcept 
{
    if( !this_t::has_current_scene() ) return false ;
    funk( this_t::get_current_scene() ) ;
    return true ;
}

//******************************************************************************************************
demos::iscene_mtr_t the_app::get_current_scene( void_t ) noexcept 
{    
    assert( _cur_scene_idx != size_t(-1) && "There must be a scene. At least one." ) ;
    return _scenes[_cur_scene_idx].s ;
}

//******************************************************************************************************
void_t the_app::on_init( void_t ) noexcept
{
    _db = motor::shared( motor::io::database( motor::io::path_t( DATAPATH ), "./working", "data" ) ) ;
    
    if( this_t::is_tool_mode() )
    {
        // tool window
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

        // debug window
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

        pr.init( "prims_for_debug" ) ;
    }
    else // production window
    {
        motor::application::window_info_t wi ;
        wi.x = 100 ;
        wi.y = 100 ;
        wi.w = 1280 ;
        wi.h = 960 ;
        wi.gen = motor::application::graphics_generation::gen4_auto ;

        _rwid = this_t::create_window( wi ) ;
        this_t::send_window_message( _rwid, [&] ( motor::application::app::window_view & wnd )
        {
            wnd.send_message( motor::application::show_message( { true } ) ) ;
            wnd.send_message( motor::application::cursor_message_t( { true } ) ) ;
            wnd.send_message( motor::application::vsync_message_t( { true } ) ) ;
        } ) ;
    }

    {
        _camera.set_dims( 1.0f, 1.0f, 1.0f, 10000.0f ) ;
        _camera.perspective_fov( motor::math::angle<float_t>::degree_to_radian( 45.0f ) ) ;
        _camera.set_sensor_dims( float_t( 1920 ), float_t( 1080 ) ) ;
        _camera.look_at( motor::math::vec3f_t( 0.0f, 0.0f, -500.0f ),
            motor::math::vec3f_t( 0.0f, 1.0f, 0.0f ), motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ;
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
        
        _db->load( motor::io::location_t( "shaders.post_process.color_to_screen.msl" ) ).wait_for_operation( 
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
                var->set( "the_scene_0.2" ) ;
            }

            mslo.add_variable_set( motor::shared( std::move( vars ), "a variable set" ) ) ;
        }

        _post_msl = motor::shared( std::move( mslo ) ) ;
    }

    // post shader
    {
        motor::graphics::msl_object_t mslo( "xfade_to_screen" ) ;

        _db->load( motor::io::location_t( "shaders.post_process.xfade_to_screen.msl" ) ).wait_for_operation(
            [&] ( char_cptr_t data, size_t const sib, motor::io::result const loading_res )
        {
            if ( loading_res != motor::io::result::ok )
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
                auto * var = vars.texture_variable( "tx_0_map" ) ;
                var->set( "the_scene_0.2" ) ;
            }

            {
                auto * var = vars.texture_variable( "tx_1_map" ) ;
                var->set( "the_scene_1.2" ) ;
            }

            {
                auto * var = vars.data_variable<float_t>( "u_overlap" ) ;
                var->set( 0.5f ) ;
            }

            mslo.add_variable_set( motor::shared( std::move( vars ), "a variable set" ) ) ;
        }

        _post_xfade_msl = motor::shared( std::move( mslo ) ) ;
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
            rss.depth_s.ss.do_depth_write = false ;
            rss.polygon_s.do_change = true ;
            rss.polygon_s.ss.do_activate = true ;
            rss.polygon_s.ss.ff = motor::graphics::front_face::clock_wise ;
            rss.polygon_s.ss.cm = motor::graphics::cull_mode::back ;
            
            rss.clear_s.do_change = false ;
            rss.clear_s.ss.clear_color = motor::math::vec4f_t( 0.5f, 0.2f, 0.2f, 1.0f ) ;
            rss.clear_s.ss.do_activate = false ;
            rss.clear_s.ss.do_color_clear = true ;
            rss.clear_s.ss.do_depth_clear = true ;

            so.add_render_state_set( rss ) ;
        }

        _pr_rs = std::move( so ) ;
    }

    {
        motor::graphics::state_object_t so = motor::graphics::state_object_t(
            "debug scene root render states" ) ;

        {
            motor::graphics::render_state_sets_t rss ;
            rss.depth_s.do_change = true ;
            rss.depth_s.ss.do_activate = true ;
            rss.depth_s.ss.do_depth_write = true ;
            rss.polygon_s.do_change = true ;
            rss.polygon_s.ss.do_activate = true ;
            rss.polygon_s.ss.ff = motor::graphics::front_face::counter_clock_wise ;
            rss.polygon_s.ss.cm = motor::graphics::cull_mode::back ;
            rss.polygon_s.ss.fm = motor::graphics::fill_mode::fill ;

            rss.clear_s.do_change = true ;
            rss.clear_s.ss.clear_color = motor::math::vec4f_t( 0.5f, 0.2f, 0.2f, 1.0f ) ;
            rss.clear_s.ss.do_activate = true ;
            rss.clear_s.ss.do_color_clear = true ;
            rss.clear_s.ss.do_depth_clear = true ;

            so.add_render_state_set( rss ) ;
        }

        _dv_rs = std::move( so ) ;
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
        {
            pp_fb0 = motor::graphics::framebuffer_object_t( "the_scene_0" ) ;
            pp_fb0.set_target( motor::graphics::color_target_type::rgba_uint_8, 3 )
                .set_target( motor::graphics::depth_stencil_target_type::depth32 )
                .resize( size_t( fb_dims.z() ), size_t( fb_dims.w() ) ) ;
        }
        {
            pp_fb1 = motor::graphics::framebuffer_object_t( "the_scene_1" ) ;
            pp_fb1.set_target( motor::graphics::color_target_type::rgba_uint_8, 3 )
                .set_target( motor::graphics::depth_stencil_target_type::depth32 )
                .resize( size_t( fb_dims.z() ), size_t( fb_dims.w() ) ) ;
        }
    }

    // init scenes
    {
        {
            auto const s = motor::math::time::to_milli( 0, 0, 0 ) ;
            auto const e = motor::math::time::to_milli( 0, 30, 0 ) ;
            _scenes.emplace_back( this_t::scene_data
                { /*true,*/ demos::scene_state::raw, demos::graphics_state::raw, demos::graphics_state::raw, 
                motor::shared( demos::scene_0( "scene_0", _dm ) ) } ) ;
        }
        
        #if 0 
        {
            auto const s = motor::math::time::to_milli( 0, 27, 0 ) ;
            auto const e = motor::math::time::to_milli( 0, 70, 0 ) ;
            _scenes.emplace_back( this_t::scene_data
                { /*false,*/ demos::scene_state::raw, demos::graphics_state::raw, demos::graphics_state::raw,
                motor::shared( demos::scene_1( "scene_1", _dm ) ) } ) ;
        }
        #endif
        for( auto & s : _scenes )
        {
            s.s->on_init_cameras() ;
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
            motor::log::global_t::status( "[my_app] : production window created" ) ;
        else
            motor::log::global_t::status( "[my_app] : window created" ) ;
    }
    if ( sv.close_changed )
    {
        motor::log::global_t::status( "[my_app] : window closed" ) ;

        if ( _rwid == wid )
        {
            _rwid = size_t( -1 ) ;
            this_t::access_current_scene_data( [&] ( this_t::scene_data & sd )
            {
                sd.ss_prod = demos::graphics_state::init ;
            } ) ;

            if( !this_t::is_tool_mode() ) 
                this->close() ;
        }
        else
        {
            this->close() ;
        }
    }
    if ( sv.resize_changed )
    {
        auto const w = uint_t( sv.resize_msg.w ) ;
        auto const h = uint_t( sv.resize_msg.h ) ;

        if( wid == _dwid )
        {
            for( auto & s : _scenes ) s.s->on_resize_debug( w, h ) ;

            if( _rwid == size_t(-1) ) 
                for( auto & s : _scenes ) s.s->on_resize( w, h ) ;
        }
        else if( wid == _rwid && w != 0 )
        {
            for( auto & s : _scenes ) s.s->on_resize( w, h ) ;
        }
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
            
        }
        else if ( keyboard.get_state( key_t::k_2 ) ==
            motor::controls::components::key_state::released )
        {
            
        }
        else if ( keyboard.get_state( key_t::k_3 ) ==
            motor::controls::components::key_state::released )
        {
            
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

        else if( keyboard.get_state( key_t::c ) == motor::controls::components::key_state::released )
        {
            if( keyboard.get_state( key_t::ctrl_left) == motor::controls::components::key_state::pressing )
            {
                this_t::get_current_scene()->camera_manager().clear_storage_data() ;
            }
            else
            {
                this_t::get_current_scene()->camera_manager().append_current_free_camera() ;
            }
            
        }
        else if( keyboard.get_state( key_t::p ) == motor::controls::components::key_state::released )
        {
            motor::string_t text = this_t::make_camera_data_file() ;
            _db->store( motor::io::location_t( "coords.txt" ), text.c_str(), text.size() ).wait_for_operation( [=] ( motor::io::result const res )
            {
                if( res != motor::io::result::ok )
                {
                    motor::log::global_t::status( "[Demo] : Coords written to file." ) ;
                    
                }
                else
                {
                    motor::log::global_t::error( "[Demo] : Coords may NOT be written to file. An error occured." ) ;
                }
            } ) ;
        }

        else if( keyboard.get_state( key_t::space ) == motor::controls::components::key_state::released )
        {
            _space_bar_pressed = true ;
        }
    }
}

//******************************************************************************************************
void_t the_app::on_update( motor::application::app::update_data_in_t ud ) noexcept
{
    if ( _proceed_time ) _cur_time += ud.milli_dt ;

    // check for every scene is it is in preload range
    // if so init or release the scene
    {
        for ( size_t i=0; i<_scenes.size(); ++i )
        {
            auto & sd = _scenes[i] ;

            bool_t const in_range = sd.s->is_in_preload_time_range( _cur_time ) ;
            bool_t const in_tool_mode = this_t::is_tool_mode() ;

            if( in_range && sd.ss == demos::scene_state::raw )
            {
                // maybe we have to wait until the graphics obects
                // are properly released.also
                bool_t const can_init = 
                    (sd.ss_prod == demos::graphics_state::raw) &&
                    (sd.ss_dbg == demos::graphics_state::raw) ;

                if( can_init )
                {
                    sd.ss = demos::scene_state::in_transit ;

                    auto the_task = motor::shared( motor::concurrent::task_t( 
                        [=] ( motor::concurrent::task_t::task_funk_param const & )
                    {
                        size_t const idx = i ;
                        this->_scenes[ idx ].s->on_init( _db ) ;
                        this->_scenes[ idx ].ss = demos::scene_state::init ;
                        // there is no more transition function, so ...
                        // ... the scene itself is ready.
                        this->_scenes[ idx ].ss = demos::scene_state::ready ;
                    } ) ) ;
                    motor::concurrent::global::schedule( motor::move( the_task ),
                            motor::concurrent::schedule_type::loose ) ;
                }
            }
            else if( !in_range && sd.ss == demos::scene_state::ready )
            {
                // we can not releas the scene until the graphics data
                // has been released!
                bool_t const can_deinit = 
                    (sd.ss_prod == demos::graphics_state::raw) && 
                    (sd.ss_dbg == demos::graphics_state::raw) ;

                if( can_deinit )
                {
                    sd.ss = demos::scene_state::in_transit ;

                    auto the_task = motor::shared( motor::concurrent::task_t(
                        [=] ( motor::concurrent::task_t::task_funk_param const & )
                    {
                        size_t const idx = i ;
                        this->_scenes[ idx ].s->on_release() ;
                        this->_scenes[ idx ].ss = demos::scene_state::raw ;
                    } ) ) ;

                    motor::concurrent::global::schedule( motor::move( the_task ),
                            motor::concurrent::schedule_type::loose ) ;
                }
                else
                {
                    // tell to deinit render objects
                    if( sd.ss_dbg == demos::graphics_state::ready )
                    {
                        sd.ss_dbg = demos::graphics_state::deinit_triggered ;
                    }

                    if( sd.ss_prod == demos::graphics_state::ready )
                    {
                        sd.ss_prod = demos::graphics_state::deinit_triggered ;
                    }
                }
            }
            
            #if 0
            if( in_range && sd.ss_dbg == demos::scene_state::raw )
            {
                sd.ss_dbg = demos::scene_state::in_transit ;

                auto the_task = motor::shared( motor::concurrent::task_t( 
                    [=] ( motor::concurrent::task_t::task_funk_param const & )
                {
                    size_t const idx = i ;
                    this->_scenes[ idx ].s->on_init( _db ) ;
                    this->_scenes[ idx ].ss_dbg = demos::scene_state::init ;
                    this->_scenes[ idx ].ss_prod = demos::scene_state::init ;
                } ) ) ;

                motor::concurrent::global::schedule( motor::move( the_task ),
                    motor::concurrent::schedule_type::loose ) ;
            }
            else if( !in_range )
            {
                {
                    if( sd.ss_dbg == demos::scene_state::ready )
                        sd.ss_dbg = demos::scene_state::render_deinit_trigger ;

                    if( sd.ss_prod == demos::scene_state::ready )
                        sd.ss_prod = demos::scene_state::render_deinit_trigger ;
                }

                if( this_t::is_tool_mode() )
                {
                    bool_t const cond =
                        sd.ss_dbg == demos::scene_state::render_deinit ||
                        sd.ss_dbg == demos::scene_state::init ;

                    if ( cond )
                    {
                        sd.ss_dbg = demos::scene_state::in_transit ;

                        auto the_task = motor::shared( motor::concurrent::task_t(
                            [=] ( motor::concurrent::task_t::task_funk_param const & )
                        {
                            size_t const idx = i ;
                            this->_scenes[ idx ].s->on_release() ;
                            this->_scenes[ idx ].ss_dbg = demos::scene_state::raw ;
                            this->_scenes[ idx ].ss_prod = demos::scene_state::raw ;
                        } ) ) ;

                        motor::concurrent::global::schedule( motor::move( the_task ),
                            motor::concurrent::schedule_type::loose ) ;
                    }
                }
                else
                {
                    bool_t const cond =
                        sd.ss_prod == demos::scene_state::render_deinit ||
                        sd.ss_prod == demos::scene_state::init ;

                    if ( cond )
                    {
                        sd.ss_prod = demos::scene_state::in_transit ;

                        auto the_task = motor::shared( motor::concurrent::task_t(
                            [=] ( motor::concurrent::task_t::task_funk_param const & )
                        {
                            size_t const idx = i ;
                            this->_scenes[ idx ].s->on_release() ;
                            this->_scenes[ idx ].ss_prod = demos::scene_state::raw ;
                        } ) ) ;

                        motor::concurrent::global::schedule( motor::move( the_task ),
                            motor::concurrent::schedule_type::loose ) ;
                    }
                }
            }
            #endif
        } 
    }

    // determine scene to be played
    // and the next scene
    {
        this_t::determine_scene_index() ;
    }

    // if in production mode, the demo should start as soon
    // as all the data is initialized.
    if( !this_t::is_tool_mode() && !_proceed_time )
    {
        auto const [a,b] = this_t::current_scene_idx() ;
        if( _scenes[a].ss == demos::scene_state::ready && 
            _scenes[a].ss_prod == demos::graphics_state::ready )
            _proceed_time = true ;
    }

    // update all scenes
    {
        for( auto & s : _scenes )
        {
            bool_t const can_update = 
                (s.ss == demos::scene_state::ready) && 
                s.s->is_in_preload_time_range( _cur_time ) ;

            if( can_update ) 
            {
                s.s->on_update( _cur_time ) ;
            }
        }
    }
}

//******************************************************************************************************
void_t the_app::on_shutdown( void_t ) noexcept 
{
    motor::release( motor::move( _post_quad ) ) ;
    motor::release( motor::move( _post_msl ) ) ;
    motor::release( motor::move( _post_xfade_msl ) ) ;
    motor::release( motor::move( _mon ) ) ;
    motor::release( motor::move( _db )  ) ;

    for( auto & s : _scenes )
    {
        if( s.ss == demos::scene_state::ready )
        {
            s.s->on_release() ;
        }
        motor::release( motor::move( s.s ) ) ;
    }
}

//******************************************************************************************************
motor::string_t the_app::make_camera_data_file( void_t ) noexcept 
{
    char buffer[2048] ;

    motor::string_t file = "Camera Data:\n" ;
    file += "-------------------------------------------------\n" ; 
    size_t i = 0 ;
    for ( auto & s : _scenes )
    {
        file += "Scene " + motor::to_string( i++ ) + "\n" ;
        
        // position
        {
            file += "Position:\n" ;
            file += "using kfs_t = demos::camera_data::keyframe_sequencef_t ;\n" ;
            file += "kfs_t kf ;\n" ;

            s.s->camera_manager().for_each( [&] ( demos::store_camera_data const & sd )
            {
                std::snprintf( buffer, 2048,
                    "kf.insert( kfs_t::keyframe_t( 123456,  motor::math::vec3f_t(%ff, %ff, %ff) ) ) ;\n",
                    sd.pos.x(), sd.pos.y(), sd.pos.z() ) ;
                file += buffer ;
                buffer[ 0 ] = '\0' ;
            } ) ;
        }

        // lookat
        {
            file += "Lookat:\n" ;
            file += "using kfs_t = demos::camera_data::keyframe_sequencef_t ;\n" ;
            file += "kfs_t kf ;\n" ;

            s.s->camera_manager().for_each( [&] ( demos::store_camera_data const & sd )
            {
                std::snprintf( buffer, 2048,
                    "kf.insert( kfs_t::keyframe_t( 123456,  motor::math::vec3f_t(%ff, %ff, %ff) ) ) ;\n ",
                    sd.lookat.x(), sd.lookat.y(), sd.lookat.z() ) ;
                file += buffer ;
                buffer[ 0 ] = '\0' ;
            } ) ;
        }

        s.s->camera_manager().clear_storage_data() ;
    }
    return file ;
}

//******************************************************************************************************
int main( int argc, char ** argv )
{
    std::srand( 127439126 );

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
