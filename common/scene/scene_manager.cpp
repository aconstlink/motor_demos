
#include "scene_manager.h"

#include <motor/tool/imgui/imgui.h>

#include <motor/concurrent/task/task.hpp>
#include <motor/concurrent/global.h>

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
}

//******************************************************************************************************
void_t scene_manager::add_scene( add_scene_data && d ) noexcept
{
    this_t::scene_data sd;

    // @todo check overlap
    sd.start = d.start;
    sd.end = d.end;
    sd.ss = demos::process_state::raw;
    sd.s = motor::move( d.sptr );

    _scenes.emplace_back( std::move( sd ) );
}

//******************************************************************************************************
void_t scene_manager::on_shutdown( void_t ) noexcept
{
    // release all scenes.

    for( auto & sd : _scenes )
    {
        motor::release( motor::move( sd.s ) );
    }
}

//******************************************************************************************************
void_t scene_manager::on_tool( void_t ) noexcept
{
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
        else if( ps == demos::process_state::ready )
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
        auto [ cur, nxt ] = this_t::determine_scene_index();

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
        }
    }
    ImGui::End();

    for( auto & sd : _scenes )
    {
        if( sd.ss != demos::process_state::init ) continue;
        sd.s->on_tool();
    }
}

//******************************************************************************************************
void_t scene_manager::for_each_scene( for_each_scene_funk_t funk ) noexcept
{
    for( auto & sd : _scenes )
    {
        funk( sd.s );
    }
}

//******************************************************************************************************
void_t scene_manager::on_scene_update( update_data_cref_t ud ) noexcept
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

            case demos::process_state::ready:
                break;
            }
        }
    }

    {
        // state handle current scene
        if( demos::is_valid( cur ) )
        {
            auto & scene = _scenes[ cur ];
            switch( scene.ss )
            {
            case demos::process_state::raw: //
            {
                scene.ss = demos::process_state::in_transit;

                auto the_task = motor::shared( motor::concurrent::task_t(
                    [ = ]( motor::concurrent::task_t::task_funk_param const & )
                {
                    this->_scenes[ cur ].s->on_init( ud.db );

                    this->_scenes[ cur ].ss = demos::process_state::init;
                } ) );
                motor::concurrent::global::schedule(
                    motor::move( the_task ), motor::concurrent::schedule_type::loose );
            }
            break;

            case demos::process_state::init:

                break;
            case demos::process_state::in_transit:
                break;
            case demos::process_state::ready:
                break;
            }
        }

        // state handle next scene
        if( demos::is_valid( nxt ) )
        {
            auto & scene = _scenes[ nxt ];
        }
    }

    // lock in the scene ids for subsequence logic
    this_t::commit_scene_index( std::make_pair( cur, nxt ) );
}

//******************************************************************************************************
void_t scene_manager::on_scene_render( render_data_ref_t rd ) noexcept
{
    scene_id_t const cur = _cur_scene_idx;
    scene_id_t const nxt = _nxt_scene_idx;

    // for each scene, approch raw state. every other scene
    // except cur and next scenes must be raw state i.e.
    // not initialized.
    this_t::approach_raw_state_graphics( rd.wt, rd.fe );
    this_t::handle_state_graphics( cur, rd.wt, rd.fe );
    this_t::handle_state_graphics( nxt, rd.wt, rd.fe );
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

        case demos::process_state::ready:
            break;
        }
    }
}

//******************************************************************************************************
void_t scene_manager::handle_state_graphics( demos::scene_id_t const id,
    demos::window_type const wt, motor::graphics::gen4::frontend_mtr_t fe )
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
        ps = demos::process_state::in_transit;
        scene.s->on_render_init( wt, fe, [ = ]( void_t )
        {
            if( wt == demos::window_type::debug )
                this->_scenes[ id ].gfx_dbg = demos::process_state::init;
            else
                this->_scenes[ id ].gfx_prod = demos::process_state::init;
        } );
    }
    break;

    case demos::process_state::init:

        break;
    case demos::process_state::in_transit:
        break;
    case demos::process_state::ready:
        break;
    }
}

//******************************************************************************************************
bool_t scene_manager::get_current_scene_info( scene_info_out_t ret ) const noexcept
{
    auto [ cur, nxt ] = this_t::current_next_scene_idx();

    if( demos::is_invalid( cur ) ) return false;

    auto & sc = _scenes[ cur ];

    ret.name = sc.s->name().c_str();
    ret.ss = sc.ss;

    return true;
}

//******************************************************************************************************
bool_t scene_manager::get_next_scene_info( scene_info_out_t ret ) const noexcept
{
    auto [ cur, nxt ] = this_t::current_next_scene_idx();

    if( demos::is_invalid( nxt ) ) return false;

    auto & sc = _scenes[ nxt ];

    ret.name = sc.s->name().c_str();
    ret.ss = sc.ss;

    return true;
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
bool_t scene_manager::get_current_scene_data( this_t::scene_data & ret ) const noexcept
{
    if( _cur_scene_idx == size_t( -1 ) ) return false;
    ret = _scenes[ _cur_scene_idx ];
    return true;
}

//******************************************************************************************************
bool_t scene_manager::access_current_scene_data(
    std::function< void_t( scene_data & sd ) > funk ) noexcept
{
    if( _cur_scene_idx == size_t( -1 ) ) return false;
    funk( _scenes[ _cur_scene_idx ] );
    return true;
}

//******************************************************************************************************
bool_t scene_manager::call_for_current_scene(
    std::function< void_t( demos::iscene_mtr_t ) > funk ) noexcept
{
    if( !this_t::has_current_scene() ) return false;
    funk( this_t::get_current_scene() );
    return true;
}

//******************************************************************************************************
demos::iscene_mtr_t scene_manager::get_current_scene( void_t ) noexcept
{
    assert( _cur_scene_idx != size_t( -1 ) && "There must be a scene. At least one." );
    return _scenes[ _cur_scene_idx ].s;
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
void_t scene_manager::commit_scene_index( demos::scene_id_pair_t const & ids ) noexcept
{
    _cur_scene_idx = ids.first;
    _nxt_scene_idx = ids.second;
}

//******************************************************************************************************
bool_t scene_manager::is_in_transition( float_t & overlap ) const noexcept
{
    size_t cur_end = 0;
    size_t nxt_srt = 0;

    if( _cur_scene_idx != size_t( -1 ) )
    {
        cur_end = _scenes[ _cur_scene_idx ].s->get_time_range().second;
    }

    if( _nxt_scene_idx == size_t( -1 ) ) return false;

    nxt_srt = _scenes[ _nxt_scene_idx ].s->get_time_range().first;
    overlap = float_t( _cur_time - nxt_srt ) / float_t( cur_end - nxt_srt );
    return true;
}
