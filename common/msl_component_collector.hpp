#pragma once

#include "types.hpp"

#include <motor/gfx/camera/generic_camera.h>

#include <motor/scene/node/node.h>
#include <motor/scene/visitor/default_visitor.h>
#include <motor/scene/component/name_component.hpp>
#include <motor/scene/component/graphics/msl_component.h>
#include <motor/scene/component/graphics/msl_set_component.hpp>

#include <motor/std/vector>
#include <motor/std/string>

namespace demos
{
using namespace motor::core::types;

// collects all the cameras in a scene graph tree
class msl_component_collector : public motor::scene::default_visitor
{
    motor_this_typedefs( msl_component_collector );

  private:

    using components_t = motor::vector< motor::scene::msl_component_mtr_t >;

    components_t _comps ;
    motor::scene::msl_set_component_t::id_t _id;

  public:

    msl_component_collector( motor::scene::msl_set_component_t::id_t const id ) : _id( id ) {}
    virtual ~msl_component_collector( void_t ) noexcept 
    {
        this_t::release() ;
    }

    virtual motor::scene::result visit( motor::scene::node_ptr_t nptr ) noexcept
    {
        motor::string_t name = "";
        {
            motor::scene::name_component_mtr_t comp;
            if( nptr->has_component_and_borrow( comp ) )
            {
                name = comp->get_name();
            }
        }
        {
            motor::scene::msl_set_component_mtr_t comp;
            if( nptr->has_component_and_borrow( comp ) )
            {
                motor::scene::msl_component_mtr_safe_t mslc ;
                if( comp->get_msl_component( _id, mslc ) )
                {
                    _comps.emplace_back( motor::move( mslc ) ) ;
                }
            }
        }

        return motor::scene::result::ok;
    }

    virtual motor::scene::result post_visit(
        motor::scene::node_ptr_t nptr, motor::scene::result const res ) noexcept
    {
        return motor::scene::result::ok;
    }

    virtual void_t on_start( void_t ) noexcept
    {
        this_t::release();
    }

    void_t for_each( std::function< void_t (motor::scene::msl_component_mtr_t) > f )
    {
        for( auto * mslc : _comps )
        {
            f( mslc ) ;
        }
    }

  public:


    void_t release( void_t ) noexcept
    {
        for( auto & i : _comps )
        {
            motor::release( motor::move( i ) ) ;
        }
        _comps.clear() ;
    }
};
motor_typedef( msl_component_collector );

} // namespace demos