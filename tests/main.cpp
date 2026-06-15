
#include "main.h"
#include "../common/the_app/the_app.h"

#include "scenes/scene_0.h"
#include "scenes/scene_1.h"

#include <motor/application/carrier.h>
#include <motor/platform/global.h>

using namespace demos;

//******************************************************************************************************
int main( int argc, char ** argv )
{
    std::srand( 127439126 );

    using namespace motor::core::types;

    #if 0
    // init scenes
    {
#if 1
        {
            auto const s = motor::math::time::to_milli( 0, 0, 0 );
            auto const e = motor::math::time::to_milli( 0, 30, 0 );
            _scenes.emplace_back(
                this_t::scene_data{ /*true,*/ demos::scene_state::raw, demos::graphics_state::raw,
                                    demos::graphics_state::raw,
                                    motor::shared( demos::scene_0( "scene_0", _dm ), "demo 0" ) } );
        }
#endif

#if 1
        {
            auto const s = motor::math::time::to_milli( 0, 27, 0 );
            auto const e = motor::math::time::to_milli( 0, 70, 0 );
            _scenes.emplace_back(
                this_t::scene_data{ /*false,*/ demos::scene_state::raw, demos::graphics_state::raw,
                                    demos::graphics_state::raw,
                                    motor::shared( demos::scene_1( "scene_1", _dm ), "demo 1" ) } );
        }
#endif

        for( auto & s : _scenes )
        {
            s.s->on_init_cameras();
        }
    }
    #endif

    motor::application::carrier_mtr_t carrier =
        motor::platform::global_t::create_carrier( motor::shared( demos::the_app() ) );

    auto const ret = carrier->exec();

    motor::memory::release_ptr( carrier );

    motor::io::global::deinit();
    motor::concurrent::global::deinit();
    motor::log::global::deinit();
    motor::memory::global::dump_to_std();

    return ret;
}
