

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


        std::pair< motor::math::time_ms_t, motor::math::time_ms_t >
        get_time_range( void_t ) const noexcept
        {
            return std::make_pair( start, end );
        }
    };
    motor_typedef( scene_data );

    motor::vector< scene_data_t > _scenes;

    // determined during on_update
    demos::scene_id_t _cur_scene_idx = demos::invalid_scene_id ;
    demos::scene_id_t _nxt_scene_idx = demos::invalid_scene_id ;

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
        motor::io::database_ptr_t db ;

        // the time the demo is in currently
        motor::math::time_ms_t demo_time;

        // some running time to keep animations
        // running that needs it
        motor::math::time_ms_t cont_time;
    };
    motor_typedef( update_data );

    // update the scenes state and call
    // scene callbacks appropriately
    void_t on_scene_update( update_data_cref_t ) noexcept;

  public:

    // for the scene manager itself
    void_t on_init( void_t ) noexcept;

    // for the scene manager itself
    void_t on_tool( void_t ) noexcept;

    // for the scene manager itself
    void_t on_shutdown( void_t ) noexcept;

  public:

    using for_each_scene_funk_t = std::function< void_t( demos::iscene_mtr_t ) >;
    void_t for_each_scene( for_each_scene_funk_t ) noexcept;

  public:

    struct scene_info
    {
        char_cptr_t name ;
        demos::scene_state ss ;
    };
    motor_typedef( scene_info ) ;
    
    bool_t get_current_scene_info( scene_info_out_t ) const noexcept ;
    bool_t get_next_scene_info( scene_info_out_t ) const noexcept ;



  private:

    bool_t is_in_time_range( this_t::scene_data_cref_t,
                             motor::math::time_ms_t const ) const noexcept;

    demos::scene_id_pair_t current_next_scene_idx( void_t ) const noexcept
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
    demos::scene_id_pair_t determine_scene_index( void_t ) noexcept;
    void_t commit_scene_index( demos::scene_id_pair_t const & ) noexcept ;


    // returns true if two scenes are transitioning and
    // if so, overlap is set. Otherwise false is returned.
    bool_t is_in_transition( float_t & overlap ) const noexcept;

    bool_t get_current_scene_data( this_t::scene_data & ) const noexcept;
    bool_t access_current_scene_data( std::function< void_t( scene_data & sd ) > ) noexcept;
};
motor_typedef( scene_manager );

} // namespace demos