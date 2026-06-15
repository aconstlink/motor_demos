
#pragma once

namespace demos
{
enum class demo_mode
{
    tool,
    production
};

// per scene
enum class scene_state
{
    raw,
    in_transit,
    init,
    ready,
    num_states
};

// per scene and per window
// possible state transitions
// raw -> init_triggered -> init -> ready
// ready/init -> deinit_triggered -> raw
enum class graphics_state
{
    raw,
    init_triggered,
    init,
    ready,
    deinit_triggered,
    num_states
};
} // namespace demos