#pragma once

#include "types.hpp"

#include <motor/gfx/camera/generic_camera.h>

#include <motor/scene/node/node.h>
#include <motor/scene/visitor/default_visitor.h>
#include <motor/scene/component/name_component.hpp>
#include <motor/scene/component/camera_component.h>

#include <motor/std/vector>
#include <motor/std/string>

namespace demos
{
using namespace motor::core::types;

// collects all the cameras in a scene graph tree
class camera_collector : public motor::scene::default_visitor
{
    motor_this_typedefs( camera_collector );

  private:

    using camera_entry_t = std::pair< motor::string_t, motor::gfx::generic_camera_mtr_t >;
    using cameras_t = motor::vector< camera_entry_t >;

    cameras_t _cameras;

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
            motor::scene::camera_component_mtr_t comp;
            if( nptr->has_component_and_borrow( comp ) )
            {
                this_t::camera_entry_t e( name, comp->get_camera() );
                _cameras.emplace_back( std::move( e ) );
            }
        }

        return motor::scene::result::ok;
    }

    virtual motor::scene::result post_visit(
        motor::scene::node_ptr_t nptr, motor::scene::result const res ) noexcept
    {
        return motor::scene::result::ok;
    }

  public:

    cameras_t get_cameras( void_t ) noexcept
    {
        return _cameras;
    }

    void_t release( void_t ) noexcept
    {
        for( auto e : _cameras )
        {
            motor::release( motor::move( e.second ) );
        }
        _cameras.clear();
    }
};
motor_typedef( camera_collector );

} // namespace demos