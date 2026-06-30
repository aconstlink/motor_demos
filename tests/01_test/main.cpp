
#include "main.h"
#include "../../common/the_app/the_app.h"

#include "../../common/scene/dummy_scene.hpp"
#include "../../common/scene/defered_scene.hpp"

#include <motor/application/carrier.h>
#include <motor/platform/global.h>

using namespace demos;

// tests defered scene. so a current scene is 
// cycling though a certain time range, so a loading
// scene would be possible, or the next scene is 
// defered until the it is loaded properly. 
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

            asd.start = motor::math::time::to_milli( 0, 0, 0 );
            asd.end = motor::math::time::to_milli( 0, 2, 0 );
            asd.sptr = motor::shared( demos::dummy_scene( "Dummy scene 1" ) );

            sm.add_scene( std::move( asd ) );
        }
#endif

#if 1
        {
            demos::scene_manager_t::add_scene_data asd;

            asd.start = motor::math::time::to_milli( 0, 2, 0 );
            asd.end = motor::math::time::to_milli( 0, 4, 0 );
            asd.sptr = motor::shared( demos::dummy_scene( "Dummy scene 2" ) );

            sm.add_scene( std::move( asd ) );
        }
#endif

#if 1
        {
            demos::scene_manager_t::add_scene_data asd;

            asd.start = motor::math::time::to_milli( 0, 3, 0 );
            asd.end = motor::math::time::to_milli( 0, 10, 0 );
            asd.sptr = motor::shared( demos::defered_scene( "Defered scene 3" ) );

            sm.add_scene( std::move( asd ) );
        }
#endif

#if 1
        {
            demos::scene_manager_t::add_scene_data asd;

            asd.start = motor::math::time::to_milli( 0, 8, 0 );
            asd.end = motor::math::time::to_milli( 0, 20, 0 );
            asd.sptr = motor::shared( demos::dummy_scene( "Dummy scene 4" ) );

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
