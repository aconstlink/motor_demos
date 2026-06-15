

#pragma once

#include "types.hpp"
#include "iscene.h"

namespace demos
{

class scene_manager
{
    motor_this_typedefs( scene_manager );

  private:

    struct scene_data
    {
        // should the scene render in
        // debug window?
        // bool_t render_in_debug = false ;

        demos::scene_state ss;
        demos::graphics_state ss_dbg;
        demos::graphics_state ss_prod;
        demos::iscene_mtr_t s;
    };
    motor::vector< scene_data > _scenes;

    // determined during on_update
    size_t _cur_scene_idx = size_t( -1 );
    size_t _nxt_scene_idx = size_t( -1 );

    std::pair< size_t, size_t > current_scene_idx( void_t ) const noexcept
    {
        return std::make_pair( _cur_scene_idx, _nxt_scene_idx );
    }

    // this function is only used in debug mode.
    // dont use for production
    demos::iscene_mtr_t get_current_scene( void_t ) noexcept;

    bool_t call_for_current_scene( std::function< void_t( demos::iscene_mtr_t ) > ) noexcept;

    

    // it can be fase, if there is no scene is in range for
    // the current time point.
    bool_t has_current_scene( void_t ) const noexcept
    {
        return _cur_scene_idx != size_t( -1 );
    }

    #if 0
    // called during on_update.
    void_t determine_scene_index( void_t ) noexcept
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
    #endif 

    #if 0
    // returns true if two scenes are transitioning and
    // if so, overlap is set. Otherwise false is returned.
    bool_t is_in_transition( float_t & overlap ) const noexcept
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
    #endif

    bool_t get_current_scene_data( this_t::scene_data & ) const noexcept;
    bool_t access_current_scene_data( std::function< void_t( scene_data & sd ) > ) noexcept;

  public:

    scene_manager( void_t ) noexcept;
    ~scene_manager( void_t ) noexcept;

  public:

    void_t add_scene( demos::iscene_mtr_safe_t ) noexcept;

  public:

    struct update_data
    {
    };
    motor_typedef( update_data );

    void_t update( update_data_cref_t ) noexcept;
};
motor_typedef( scene_manager );

} // namespace demos