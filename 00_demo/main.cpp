
#include "main.h"
#include "../common/the_app/the_app.h"

#include "scenes/00_intro_scene.h"
#include "scenes/01_scene.h"
#include "scenes/02_scene.h"
#include "scenes/03_outro_scene.h"

#include "../common/scene/dummy_scene.hpp"
#include "../common/scene/simple_gltf_scene.hpp"

#include <motor/application/carrier.h>
#include <motor/platform/global.h>

using namespace demos;

//******************************************************************************************************
int main( int argc, char ** argv )
{
    std::srand( 127439126 );

    using namespace motor::core::types;

    demos::the_app_mtr_t app = nullptr;

    // init scenes
    {
        demos::scene_manager_t sm;
#if 1
        {
            demos::scene_manager_t::add_scene_data asd;
            asd.sptr = motor::shared( demos::intro_scene( "intro" ) ); 

            sm.add_scene( std::move( asd ) );
        }
#endif

        app = motor::shared( demos::the_app( std::move( sm ) ) );
    }

    motor::application::carrier_mtr_t carrier =
        motor::platform::global_t::create_carrier( motor::move( app ) );

    auto const ret = carrier->exec();

    motor::memory::release_ptr( carrier );

    motor::io::global::deinit();
    motor::concurrent::global::deinit();
    motor::log::global::deinit();

    motor::memory::global::dump_to_std();

    return ret;
}
