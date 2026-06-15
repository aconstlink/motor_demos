

#pragma once

#include "../types.hpp"
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

        motor::math::time_ms_t start;
        motor::math::time_ms_t end;

        demos::iscene_mtr_t s;
    };
    motor::vector< scene_data > _scenes;

    // determined during on_update
    size_t _cur_scene_idx = size_t( -1 );
    size_t _nxt_scene_idx = size_t( -1 );

    size_t _cur_time = 0;

  private:

    std::pair< size_t, size_t > current_scene_idx( void_t ) const noexcept
    {
        return std::make_pair( _cur_scene_idx, _nxt_scene_idx );
    }

    // this function is only used in debug mode.
    // dont use for production
    demos::iscene_mtr_t get_current_scene( void_t ) noexcept;

    bool_t call_for_current_scene( std::function< void_t( demos::iscene_mtr_t ) > ) noexcept;

    // it can be false, if there is no scene in range for
    // the current time point.
    bool_t has_current_scene( void_t ) const noexcept
    {
        return _cur_scene_idx != size_t( -1 );
    }

    // called during on_update.
    void_t determine_scene_index( void_t ) noexcept;

    // returns true if two scenes are transitioning and
    // if so, overlap is set. Otherwise false is returned.
    bool_t is_in_transition( float_t & overlap ) const noexcept;

    bool_t get_current_scene_data( this_t::scene_data & ) const noexcept;
    bool_t access_current_scene_data( std::function< void_t( scene_data & sd ) > ) noexcept;

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
    };
    motor_typedef( update_data );

    void_t update( update_data_cref_t ) noexcept;

  public:

    void_t on_tool( void_t ) noexcept;
    void_t on_shutdown( void_t ) noexcept;

  public:

    using for_each_scene_funk_t = std::function< void_t( demos::iscene_mtr_t ) >;
    void_t for_each_scene( for_each_scene_funk_t ) noexcept;
};
motor_typedef( scene_manager );

} // namespace demos