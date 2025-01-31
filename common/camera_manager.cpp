
#include "camera_manager.h"

using namespace demos ;

//*********************************************************
void_t camera_manager::append_current_free_camera( void_t ) noexcept 
{
    this_t::append( {_free_cam.pos, _free_cam.lookat } ) ;
}

//*********************************************************
void_t camera_manager::append( store_camera_data && ss  ) noexcept
{
    _storage.emplace_back( std::move( ss )  ) ;
}

//*********************************************************
void_t camera_manager::for_each( std::function< void_t ( store_camera_data const & ) > f ) noexcept 
{
    for( auto const & s : _storage ) 
    {
        f( s ) ;
    }
}

//*********************************************************
void_t camera_manager::clear_storage_data( void_t ) noexcept 
{
    _storage.clear() ;
}