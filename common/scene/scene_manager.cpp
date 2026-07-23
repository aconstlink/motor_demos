
#include "scene_manager.h"

#include <motor/tool/util.hpp>
#include <motor/tool/imgui/imgui.h>

#include <motor/concurrent/task/task.hpp>
#include <motor/concurrent/global.h>

#include <motor/tool/imgui/imgui_property.h>

using namespace demos;

//******************************************************************************************************
scene_manager::scene_manager( void_t ) noexcept {}

//******************************************************************************************************
scene_manager::scene_manager( this_rref_t rhv ) noexcept : _scenes( std::move( rhv._scenes ) ) {}

//******************************************************************************************************
scene_manager::~scene_manager( void_t ) noexcept
{
    for( auto & s : _scenes )
    {
        motor::release( motor::move( s.s ) );
    }

    motor::release( motor::move( _db ) );
}

//******************************************************************************************************
void_t scene_manager::add_scene( add_scene_data && d ) noexcept
{
    this_t::scene_data sd;

    motor::math::time_ms_t start = 0;

    if( _scenes.size() > 0 )
    {
        start = std::min( _scenes.back().end, _scenes.back().end - 2000 );
    }

    // @todo check overlap
    sd.start = start;
    sd.end = start + d.sptr->get_timing_info().duration;
    sd.ss = demos::process_state::raw;
    sd.s = motor::move( d.sptr );

    _scenes.emplace_back( std::move( sd ) );
}

//******************************************************************************************************
motor::math::time_ms_t scene_manager::get_whole_duration( void_t ) const noexcept
{
    return _scenes.size() > 0 ? _scenes.back().end : 0;
}

//******************************************************************************************************
void_t scene_manager::on_init( this_t::init_data & idata ) noexcept
{
    _db = motor::move( idata.db );

// remember for later!
#if 0
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
#endif

    {
        _pp_pipe = motor::shared( motor::gfx::hdr_postprocess_pipeline_t() );
        _pp_pipe->init();
    }
}

//******************************************************************************************************
void_t scene_manager::on_event( this_t::event_data_in_t ed ) noexcept
{
    if( ed.window_size_changed )
    {
        for( auto & s : _scenes )
        {
            if( s.ss == demos::process_state::init )
            {
                s.s->on_resize( ed.wt, ed.window_dims.x(), ed.window_dims.y() );
            }
        }
    }
}

//******************************************************************************************************
void_t scene_manager::on_shutdown( void_t ) noexcept
{
    // release all scenes.

    for( auto & sd : _scenes )
    {
        motor::release( motor::move( sd.s ) );
    }

    _pp_pipe->release();
    motor::release( motor::move( _pp_pipe ) );
}

//******************************************************************************************************
void_t scene_manager::on_tool( void_t ) noexcept
{
    auto [ cur, nxt ] = this_t::get_scene_index();

    auto color_chooser_funk = [ & ]( demos::process_state const ps )
    {
        ImVec4 color( 0.0f, 0.0f, 0.0f, 1.0 );
        if( ps == demos::process_state::in_transit )
        {
            color = ImVec4( 1.0f, 1.0f, 0.0f, 1.0f );
        }
        else if( ps == demos::process_state::init )
        {
            color = ImColor( 0, 255, 0, 255 );
        }
        else if( ps == demos::process_state::raw )
        {
            color = ImColor( 255, 0, 0, 255 );
        }

        return color;
    };

    if( ImGui::Begin( "Scene Manager" ) )
    {
        for( size_t i = 0; i < _scenes.size(); ++i )
        {
            auto & s = _scenes[ i ];

            char buffer[ 4096 ];
            std::snprintf( buffer, 4096, "%s%s", i == cur ? "*" : " ", s.s->name().c_str() );

            ImGui::Text( buffer );
            ImGui::SameLine();

            {
                auto color = color_chooser_funk( s.ss );
                std::memset( buffer, 0, sizeof( buffer ) );
                std::snprintf( buffer, 4096, "##process_state_%s", s.s->name().c_str() );
                ImGui::SameLine();
                ImGui::ColorButton( buffer, color );
            }

            {
                auto color = color_chooser_funk( s.gfx_dbg );
                std::memset( buffer, 0, sizeof( buffer ) );
                std::snprintf(
                    buffer, 4096, "##process_state_debug_graphics%s", s.s->name().c_str() );
                ImGui::SameLine();
                ImGui::ColorButton( buffer, color );
            }

            {
                auto color = color_chooser_funk( s.gfx_prod );
                std::memset( buffer, 0, sizeof( buffer ) );
                std::snprintf(
                    buffer, 4096, "##process_state_prod_graphics%s", s.s->name().c_str() );
                ImGui::SameLine();
                ImGui::ColorButton( buffer, color );
            }

            // time
            {
                // start
                {
                    ImGui::SameLine();
                    motor::tool::make_time_string( buffer, sizeof( buffer ), s.start );
                    ImGui::Text( buffer );
                }
                {
                    ImGui::SameLine();
                    std::memset( buffer, 0, sizeof( buffer ) );
                    std::snprintf( buffer, 4096, "|" );
                    ImGui::Text( buffer );
                }
                // end
                {
                    ImGui::SameLine();
                    motor::tool::make_time_string( buffer, sizeof( buffer ), s.end );
                    ImGui::Text( buffer );
                }
            }
        }
    }
    ImGui::End();

    if( ImGui::Begin( "Post Process" ) )
    {
        bool_t has_changed = false;
        for( auto ps : _pp_pipe->property_sheets() )
        {
            has_changed |= motor::tool::imgui_property::handle( ps.first, *ps.second );
        }
        if( has_changed ) _pp_pipe->update_properies();

        {
            if( ImGui::Checkbox( "Show temp render target ##scene_manager", &_show_temp_rt ) )
            {
                _pp_pipe->set_map_to_screen_texture_temp("scene.00.shadow_framebuffer.depth") ;
            }
        }
    }
    ImGui::End();

    {
        for( auto & sd : _scenes )
        {
            if( sd.ss != demos::process_state::init ) continue;

            char buffer[ 4096 ];
            std::snprintf( buffer, 4096, "%s", sd.s->name().c_str() );

            if( ImGui::Begin( buffer ) )
            {
                sd.s->on_tool();
            }
            ImGui::End();
        }
    }
}

//******************************************************************************************************
motor::math::time_ms_t scene_manager::on_scene_update( update_data_cref_t ud ) noexcept
{
    _cur_time = ud.demo_time;

    auto [ cur, nxt ] = this_t::determine_scene_index();

    // for each scene, approch raw state. every other scene
    // except cur and next scenes must be raw state i.e.
    // not initialized.
    {
        for( size_t i = 0; i < _scenes.size(); ++i )
        {
            if( i == cur || i == nxt ) continue;

            auto & scene = _scenes[ i ];

            // scene -> approach raw state...
            switch( scene.ss )
            {
            case demos::process_state::raw:
            case demos::process_state::in_transit:
                // do nothing
                break;

            case demos::process_state::init: // have to release
            {
                bool_t const can_release = scene.gfx_dbg == demos::process_state::raw &&
                                           scene.gfx_prod == demos::process_state::raw;

                if( can_release )
                {
                    scene.ss = demos::process_state::in_transit;

                    auto the_task = motor::shared( motor::concurrent::task_t(
                        [ = ]( motor::concurrent::task_t::task_funk_param const & )
                    {
                        this->_scenes[ i ].s->on_release();
                        this->_scenes[ i ].ss = demos::process_state::raw;
                    } ) );
                    motor::concurrent::global::schedule(
                        motor::move( the_task ), motor::concurrent::schedule_type::loose );
                }
            }
            break;
            }
        }
    }

    {
        auto scene_handle_funk = [ = ]( demos::scene_id_t const id )
        {
            if( demos::is_invalid( id ) ) return;

            auto & scene = _scenes[ id ];
            switch( scene.ss )
            {
            case demos::process_state::raw: //
            {
                scene.ss = demos::process_state::in_transit;

                auto the_task = motor::shared( motor::concurrent::task_t(
                    [ = ]( motor::concurrent::task_t::task_funk_param const & )
                {
                    this->_scenes[ id ].s->on_init( ud.db );
                    this->_scenes[ id ].ss = demos::process_state::init;
                } ) );
                motor::concurrent::global::schedule(
                    motor::move( the_task ), motor::concurrent::schedule_type::loose );
            }
            break;

            case demos::process_state::init:
            case demos::process_state::in_transit:
                break;
            }
        };

        // state handle current scene
        scene_handle_funk( cur );

        // state handle next scene
        scene_handle_funk( nxt );
    }

    // lock in the scene ids for subsequence logic
    this_t::commit_scene_index( std::make_pair( cur, nxt ) );

    if( demos::is_valid( cur ) )
    {
        auto & cur_scene = _scenes[ cur ];

        demos::iscene::update_data_t sud;
        sud.absolute = _cur_time;
        sud.relative = _cur_time - cur_scene.start;
        sud.relative_seconds = float_t( sud.relative ) / 1000.0f;
        if( cur_scene.ss == demos::process_state::init ) cur_scene.s->on_update( sud );
    }

    // update next scene if overlapping
    // also, if next scene is not loaded yet, enable cycling through
    // a specific time range in the current scene
    // => enables loading screen naturally.
    // make the fist scenen a very simple loading scene and
    // load a next more complex scene, so the loading scene
    // will cycle back into the loading scene for a few seconds.
    {
        float_t overlap = 0.0f;
        if( this_t::is_in_transition( overlap ) )
        {
            auto & cur_scene = _scenes[ cur ];
            auto & nxt_scene = _scenes[ nxt ];

            demos::iscene::update_data_t sud;
            sud.absolute = _cur_time;
            sud.relative = _cur_time - nxt_scene.start;

            if( nxt_scene.ss == demos::process_state::init )
            {
                nxt_scene.s->on_update( sud );
            }

            // if next scene is not loaded yet, fix current time stamp
            // but dont set it back the beyond the current scenes' time range start,
            // because otherwise the next scene will be deloaded again.
            else if( nxt_scene.ss == demos::process_state::in_transit )
            {
                if( _cur_time >= nxt_scene.start )
                {
                    motor::math::time_ms_t const set_back = std::min(
                        nxt_scene.s->get_timing_info().set_back, ( _cur_time - cur_scene.start ) );
                    _cur_time = _cur_time - set_back;
                }
            }
        }
    }

    return _cur_time;
}

//******************************************************************************************************
void_t scene_manager::on_render( render_data_ref_t rd ) noexcept
{
    if( rd.wt == demos::window_type::invalid ) return;
    if( rd.wt == demos::window_type::tool ) return;

    if( rd.first_frame && rd.wt == demos::window_type::debug )
    {
    }    

    scene_id_t const cur = _cur_scene_idx;
    scene_id_t const nxt = _nxt_scene_idx;

    // for each scene, approch raw state. every other scene
    // except cur and next scenes must be raw state i.e.
    // not initialized.
    this_t::approach_raw_state_graphics( rd.wt, rd.fe );
    this_t::handle_state_graphics( cur, rd.wt, rd.fe, rd.last_frame );
    this_t::handle_state_graphics( nxt, rd.wt, rd.fe, rd.last_frame );

    if( rd.wt == demos::window_type::debug )
    {
        this_t::on_render_debug( rd );
    }
    else if( rd.wt == demos::window_type::production )
    {
        this_t::on_render_production( rd );
    }
}

//******************************************************************************************************
void_t scene_manager::on_render_debug( render_data_ref_t rd ) noexcept
{
    scene_id_t const cur = _cur_scene_idx;
    scene_id_t const nxt = _nxt_scene_idx;

    if( this_t::is_valid_and_init( cur, demos::window_type::debug ) )
    {
        _scenes[ cur ].s->on_render_debug( rd.wid, rd.fe );
    }

    // eventually render next scene if overlap
    {
        float_t overlap = 0.0f;
        if( this_t::is_in_transition( overlap ) )
        {
            auto & snxt = _scenes[ nxt ];

            if( snxt.gfx_dbg == demos::process_state::init )
            {
                snxt.s->on_render_debug( rd.wid, rd.fe );
            }
        }
    }
}

//******************************************************************************************************
void_t scene_manager::on_render_production( render_data_ref_t rd ) noexcept
{
    // init render window rendering objects
    if( rd.first_frame )
    {
        _pp_pipe->init_render( rd.fe );
    }    

    scene_id_t const cur = _cur_scene_idx;
    scene_id_t const nxt = _nxt_scene_idx;

    if( this_t::is_valid_and_init( cur, demos::window_type::production ) )
    {
        _scenes[ cur ].s->on_render_final_offscreen( rd.wid, rd.fe );

        // activate fb 0
        rd.fe->use( _pp_pipe->borrow_hdr_fb( 0 ) );
        rd.fe->push( _pp_pipe->borrow_hdr_states() );
        _scenes[ cur ].s->on_render_final( rd.wid, rd.fe );
        rd.fe->pop( motor::graphics::gen4::backend::pop_type::render_state );
        rd.fe->unuse( motor::graphics::gen4::backend::unuse_type::framebuffer );
    }

    // eventually render next scene if overlap
    float_t overlap = 0.0f;
    if( this_t::is_in_transition( overlap ) )
    {
        auto & snxt = _scenes[ nxt ];

        if( snxt.gfx_prod == demos::process_state::init )
        {
            snxt.s->on_render_final_offscreen( rd.wid, rd.fe );

            // activate fb1
            rd.fe->use( _pp_pipe->borrow_hdr_fb( 0 ) );
            rd.fe->push( _pp_pipe->borrow_hdr_states() );
            snxt.s->on_render_final( rd.wid, rd.fe );
            rd.fe->pop( motor::graphics::gen4::backend::pop_type::render_state );
            rd.fe->unuse( motor::graphics::gen4::backend::unuse_type::framebuffer );
        }
    }

    // render transition fb0 x fb1
    // or render fb0

    if( rd.last_frame )
    {
        _pp_pipe->release_render( rd.fe );
        return;
    }

    // post process
    {
        _pp_pipe->render( rd.fe, _show_temp_rt );
    }
}

//******************************************************************************************************
bool_t scene_manager::is_valid_and_init(
    demos::scene_id_t const id, demos::window_type const wt ) const noexcept
{
    if( wt == demos::window_type::debug )
    {
        if( demos::is_valid( id ) && _scenes[ id ].gfx_dbg == demos::process_state::init )
            return true;
    }
    else if( wt == demos::window_type::production )
    {
        if( demos::is_valid( id ) && _scenes[ id ].gfx_prod == demos::process_state::init )
            return true;
    }
    return false;
}

//******************************************************************************************************
void_t scene_manager::approach_raw_state_graphics(
    demos::window_type const wt, motor::graphics::gen4::frontend_mtr_t fe )
{
    scene_id_t const cur = _cur_scene_idx;
    scene_id_t const nxt = _nxt_scene_idx;

    for( size_t i = 0; i < _scenes.size(); ++i )
    {
        if( i == cur || i == nxt ) continue;

        auto & scene = _scenes[ i ];

        auto & ps = wt == demos::window_type::debug ? scene.gfx_dbg : scene.gfx_prod;

        switch( ps )
        {
        case demos::process_state::raw:
        case demos::process_state::in_transit:
            // do nothing
            break;

        case demos::process_state::init: // have to release
        {
            ps = demos::process_state::in_transit;

            scene.s->on_render_deinit( wt, fe, [ = ]( void_t )
            {
                if( wt == demos::window_type::debug )
                    this->_scenes[ i ].gfx_dbg = demos::process_state::raw;
                else
                    this->_scenes[ i ].gfx_prod = demos::process_state::raw;
            } );
        }

        break;
        }
    }
}

//******************************************************************************************************
void_t scene_manager::handle_state_graphics( demos::scene_id_t const id,
    demos::window_type const wt, motor::graphics::gen4::frontend_mtr_t fe, bool_t const do_release )
{
    if( demos::is_invalid( id ) ) return;

    auto & scene = _scenes[ id ];

    // we can only process graphics state if the scene is initialized
    if( scene.ss != demos::process_state::init ) return;

    auto & ps = wt == demos::window_type::debug ? scene.gfx_dbg : scene.gfx_prod;

    switch( ps )
    {
    case demos::process_state::raw: //
    {
        if( !do_release )
        {
            ps = demos::process_state::in_transit;
            scene.s->on_render_init( wt, fe, [ = ]( void_t )
            {
                if( wt == demos::window_type::debug )
                    this->_scenes[ id ].gfx_dbg = demos::process_state::init;
                else
                    this->_scenes[ id ].gfx_prod = demos::process_state::init;
            } );
        }
    }
    break;

    case demos::process_state::init: //
    {
        if( do_release )
        {
            ps = demos::process_state::in_transit;
            scene.s->on_render_deinit( wt, fe, [ = ]( void_t )
            {
                if( wt == demos::window_type::debug )
                    this->_scenes[ id ].gfx_dbg = demos::process_state::raw;
                else
                    this->_scenes[ id ].gfx_prod = demos::process_state::raw;
            } );
        }
    }

    break;
    case demos::process_state::in_transit:
        break;
    }
}

//
// PRIVATE
//

//******************************************************************************************************
bool_t scene_manager::is_in_time_range(
    this_t::scene_data_cref_t sd, motor::math::time_ms_t const milli ) const noexcept
{
    auto const range = sd.get_time_range();
    return milli >= range.first && milli <= range.second;
}

//******************************************************************************************************
demos::scene_id_pair_t scene_manager::determine_scene_index( void_t ) noexcept
{
    size_t cur_scene = size_t( -1 );
    size_t nxt_scene = size_t( -1 );

    // #0 : determine scene idx
    {
        while( ++cur_scene < _scenes.size() )
        {
            if( this_t::is_in_time_range( _scenes[ cur_scene ], _cur_time ) ) break;
        }

        if( cur_scene == _scenes.size() )
        {
            return std::make_pair( demos::invalid_scene_id, demos::invalid_scene_id );
        }

        if( ( cur_scene + 1 ) < _scenes.size() &&
            this_t::is_in_time_range( _scenes[ cur_scene ], _cur_time ) )
        {
            nxt_scene = cur_scene + 1;
        }
    }

    return std::make_pair( cur_scene, nxt_scene );
}

//******************************************************************************************************
demos::scene_id_pair_t scene_manager::get_scene_index( void_t ) noexcept
{
    return std::make_pair( _cur_scene_idx, _nxt_scene_idx );
}

//******************************************************************************************************
void_t scene_manager::commit_scene_index( demos::scene_id_pair_t const & ids ) noexcept
{
    _cur_scene_idx = ids.first;
    _nxt_scene_idx = ids.second;
}
