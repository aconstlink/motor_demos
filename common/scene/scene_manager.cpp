
#include "scene_manager.h"

using namespace demos;

//******************************************************************************************************
scene_manager::scene_manager( void_t ) noexcept {}

//******************************************************************************************************
scene_manager::scene_manager( this_rref_t rhv ) noexcept : _scenes( std::move( rhv._scenes ) ) {}

//******************************************************************************************************
scene_manager::~scene_manager( void_t ) noexcept {}

//******************************************************************************************************
void_t scene_manager::add_scene( add_scene_data && d ) noexcept
{
    this_t::scene_data sd;

    // @todo check overlap
    sd.start = d.start;
    sd.end = d.end;

    sd.s = motor::move( d.sptr );

    _scenes.emplace_back( std::move( sd ) );
}

//******************************************************************************************************
bool_t scene_manager::get_current_scene_data( this_t::scene_data & ret ) const noexcept
{
    if( _cur_scene_idx == size_t( -1 ) ) return false;
    ret = _scenes[ _cur_scene_idx ];
    return true;
}

//******************************************************************************************************
bool_t
scene_manager::access_current_scene_data( std::function< void_t( scene_data & sd ) > funk ) noexcept
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
void_t scene_manager::determine_scene_index( void_t ) noexcept
{
    size_t cur_scene = size_t( -1 );
    size_t nxt_scene = size_t( -1 );

    // #0 : determine scene idx
    {
        while( ++cur_scene < _scenes.size() )
        {
            if( _scenes[ cur_scene ].s->is_in_time_range( _cur_time ) ) break;
        }
        if( cur_scene == _scenes.size() ) cur_scene = size_t( -1 );

        if( ( cur_scene + 1 ) < _scenes.size() &&
            _scenes[ cur_scene + 1 ].s->is_in_time_range( _cur_time ) )
            nxt_scene = cur_scene + 1;
    }

    _cur_scene_idx = cur_scene;
    _nxt_scene_idx = nxt_scene;
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

//******************************************************************************************************
void_t scene_manager::on_shutdown( void_t ) noexcept
{
    // release all scenes.
}

//******************************************************************************************************
void_t scene_manager::on_tool( void_t ) noexcept
{
    for( auto & sd : _scenes )
    {
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