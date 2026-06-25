

#pragma once

#include "../types.hpp"
#include "iscene.h"

namespace demos
{

class scene_manager
{
    motor_this_typedefs( scene_manager );

  private:

    

  private:

    struct scene_data
    {
        // should the scene render in
        // debug window?
        // bool_t render_in_debug = false ;

        demos::process_state ss = demos::process_state::raw;
        demos::process_state gfx_dbg = demos::process_state::raw;
        demos::process_state gfx_prod = demos::process_state::raw;

        motor::math::time_ms_t start;
        motor::math::time_ms_t end;

        demos::iscene_mtr_t s;

        std::pair< motor::math::time_ms_t, motor::math::time_ms_t > get_time_range(
            void_t ) const noexcept
        {
            return std::make_pair( start, end );
        }
    };
    motor_typedef( scene_data );

    motor::vector< scene_data_t > _scenes;

    // determined during on_update
    demos::scene_id_t _cur_scene_idx = demos::invalid_scene_id;
    demos::scene_id_t _nxt_scene_idx = demos::invalid_scene_id;

    size_t _cur_time = 0;

  public:

    scene_manager( void_t ) noexcept;
    scene_manager( this_cref_t ) = delete;
    scene_manager( this_rref_t ) noexcept;
    ~scene_manager( void_t ) noexcept;

  public:

    struct add_scene_data
    {
        motor::math::time_ms_t start;
        motor::math::time_ms_t end;

        demos::iscene_mtr_t sptr;
    };

    void_t add_scene( add_scene_data && ) noexcept;

  public:

    struct update_data
    {
        motor::io::database_ptr_t db;

        // the time the demo is in currently
        motor::math::time_ms_t demo_time;

        // some running time to keep animations
        // running that needs it
        motor::math::time_ms_t cont_time;
    };
    motor_typedef( update_data );

    // update the scenes state and call
    // scene callbacks appropriately
    motor::math::time_ms_t on_scene_update( update_data_cref_t ) noexcept;

    struct render_data
    {
        size_t wid;
        motor::graphics::gen4::frontend_mtr_t fe;

        demos::window_type wt;
    };
    motor_typedef( render_data );

    void_t on_render( render_data_ref_t ) noexcept;

  public:

    // for the scene manager itself
    void_t on_init( void_t ) noexcept;

    // for the scene manager itself
    void_t on_tool( void_t ) noexcept;

    // for the scene manager itself
    void_t on_shutdown( void_t ) noexcept;

  private:

    void_t on_render_debug( render_data_ref_t ) noexcept;
    void_t on_render_production( render_data_ref_t ) noexcept;

    bool_t is_valid_and_init( demos::scene_id_t const, demos::window_type const ) const noexcept;

    bool_t is_in_time_range(
        this_t::scene_data_cref_t, motor::math::time_ms_t const ) const noexcept;

    // called during on_update.
    demos::scene_id_pair_t determine_scene_index( void_t ) noexcept;
    void_t commit_scene_index( demos::scene_id_pair_t const & ) noexcept;

    void_t approach_raw_state_graphics(
        demos::window_type const, motor::graphics::gen4::frontend_mtr_t );
    void_t handle_state_graphics(
        demos::scene_id_t const, demos::window_type const, motor::graphics::gen4::frontend_mtr_t );

    // returns true if two scenes are transitioning and
    // if so, overlap is set. Otherwise false is returned.
    bool_t is_in_transition( float_t & overlap ) const noexcept
    {
        size_t cur_end = 0;
        size_t nxt_srt = 0;

        if( _cur_scene_idx == size_t( -1 ) ) return false;
        if( _nxt_scene_idx == size_t( -1 ) ) return false;

        cur_end = _scenes[ _cur_scene_idx ].end;
        nxt_srt = _scenes[ _nxt_scene_idx ].start;

        if( _cur_time > nxt_srt && _cur_time < cur_end )
        {
            overlap = float_t( _cur_time - nxt_srt ) / float_t( cur_end - nxt_srt );
            return true;
        }
        return false;
    }

  public:

    // determine_scene_index must be used somewhere before in order to have a
    // proper scene index determination.
    demos::scene_id_pair_t get_scene_index( void_t ) noexcept;

    struct for_each_scene_info
    {
        motor::math::time_ms_t start;
        motor::math::time_ms_t end;

        demos::iscene_mtr_t s; // borrow
    };
    motor_typedef( for_each_scene_info );
    using for_each_scene_funk_t =
        std::function< void_t( size_t const idx, for_each_scene_info_in_t ) >;
    void_t for_each_scene( for_each_scene_funk_t funk ) noexcept
    {
        size_t i = 0;
        for( auto & s : _scenes )
        {
            for_each_scene_info info;
            info.end = s.end;
            info.start = s.start;
            info.s = s.s;
            funk( i++, info );
        }
    }
};
motor_typedef( scene_manager );

} // namespace demos