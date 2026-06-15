
#include "scene_manager.h"

using namespace demos ;

//******************************************************************************************************
bool_t scene_manager::get_current_scene_data( this_t::scene_data & ret ) const noexcept 
{
    if( _cur_scene_idx == size_t(-1)  ) return false ;
    ret = _scenes[_cur_scene_idx] ;
    return true ;
}

//******************************************************************************************************
bool_t scene_manager::access_current_scene_data( std::function< void_t (scene_data & sd ) > funk ) noexcept 
{
    if( _cur_scene_idx == size_t(-1)  ) return false ;
    funk( _scenes[ _cur_scene_idx ] ) ;
    return true ;
}

//******************************************************************************************************
bool_t scene_manager::call_for_current_scene( std::function< void_t ( demos::iscene_mtr_t ) > funk ) noexcept 
{
    if( !this_t::has_current_scene() ) return false ;
    funk( this_t::get_current_scene() ) ;
    return true ;
}

//******************************************************************************************************
demos::iscene_mtr_t scene_manager::get_current_scene( void_t ) noexcept 
{    
    assert( _cur_scene_idx != size_t(-1) && "There must be a scene. At least one." ) ;
    return _scenes[_cur_scene_idx].s ;
}