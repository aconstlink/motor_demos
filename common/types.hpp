
#pragma once

#include <motor/scene/component/graphics/msl_set_component.hpp>
#include <motor/base/types.hpp>

namespace demos
{
using namespace motor::core::types;

enum class demo_mode
{
    tool,
    production
};

enum class window_type
{
    invalid,
    tool,
    debug,
    production
};

// per scene
enum class process_state
{
    raw,
    in_transit,
    init,
    num_states
};

// used in the msl_set_component
enum class msl_id
{
    default_id = 0,
    final_id = 1,
    shadow_id = 2
};

static motor::scene::msl_set_component::id_t to_id( msl_id const id ) noexcept
{
    return motor::scene::msl_set_component::id_t( id ) ;
}

//
// SECTION : scene id
//

using scene_id_t = size_t;

static scene_id_t invalid_scene_id = size_t( -1 );

static scene_id_t invalid( void_t ) noexcept
{
    return invalid_scene_id;
}

static bool_t is_valid( scene_id_t const sid ) noexcept
{
    return sid != invalid_scene_id;
}

static bool_t is_invalid( scene_id_t const sid ) noexcept
{
    return sid == invalid_scene_id;
}

using scene_id_pair_t = std::pair< scene_id_t, scene_id_t >;

} // namespace demos