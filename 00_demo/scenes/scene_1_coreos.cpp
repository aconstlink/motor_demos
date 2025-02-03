
#include "scene_1.h"

#include <motor/math/utility/fn.hpp>
#include <motor/math/utility/angle.hpp>
#include <motor/concurrent/parallel_for.hpp>

using namespace demos ;

//*****************************************************************************************
void_t scene_1::coreo_0( demos::iscene::on_graphics_data_in_t gd ) noexcept
{
    _cubes_data.data_buffer().resize( this_t::num_objects() ).
        update< the_data >( [&] ( the_data * array, size_t const ne )
    {
        typedef motor::concurrent::range_1d<size_t> range_t ;
        auto const & range = range_t( 0, std::min( size_t( this_t::num_objects() ), this_t::num_objects() ) ) ;

        motor::concurrent::parallel_for<size_t>( range, [&] ( range_t const & r )
        {
        } ) ;
    } ) ;
}

//*****************************************************************************************
void_t scene_1::coreo_1( demos::iscene::on_graphics_data_in_t gd ) noexcept
{
    _cubes_data.data_buffer().resize( this_t::num_objects() ).
        update< the_data >( [&] ( the_data * array, size_t const ne )
    {
        typedef motor::concurrent::range_1d<size_t> range_t ;
        auto const & range = range_t( 0, std::min( size_t( this_t::num_objects() ), this_t::num_objects() ) ) ;

        motor::concurrent::parallel_for<size_t>( range, [&] ( range_t const & r )
        {
        } ) ;
    } ) ;
}