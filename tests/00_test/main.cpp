
#include "main.h"
#include "../../common/the_app/the_app.h"

#include "scenes/scene_0.h"
#include "scenes/scene_1.h"

#include "../../common/scene/dummy_scene.hpp"
#include "../../common/scene/simple_gltf_scene.hpp"

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
            asd.sptr = motor::shared( demos::dummy_scene( "scene 1" ) );

            sm.add_scene( std::move( asd ) );
        }
#endif

#if 1
        {
            demos::scene_manager_t::add_scene_data asd;
            asd.sptr = motor::shared(
                demos::simple_gltf_scene( "scene 2", motor::io::location_t( "scene_1.scene.gltf" ), 5000 ) );

            sm.add_scene( std::move( asd ) );
        }
#endif

#if 1
        {
            demos::scene_manager_t::add_scene_data asd;
            asd.sptr = motor::shared(
                demos::simple_gltf_scene( "scene 3", motor::io::location_t( "scene_2.scene.gltf" ), 10000 ) );

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
