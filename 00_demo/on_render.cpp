
#include "main.h"

using namespace demos ;

//******************************************************************************************************
void_t the_app::on_render( this_t::window_id_t const wid, motor::graphics::gen4::frontend_ptr_t fe,
    motor::application::app::render_data_in_t rd ) noexcept
{
    //if( wid == 0 ) return  ;

    if ( rd.first_frame )
    {
        // init debug window rendering objects
        if( wid == _dwid )
        {
            pr.configure( fe ) ;
            fe->configure< motor::graphics::state_object_t>( &_pr_rs ) ;
            fe->configure< motor::graphics::state_object_t>( &_debug_rs ) ;
            fe->configure<motor::graphics::geometry_object>( _dummy_geo ) ;
            fe->configure<motor::graphics::msl_object>( _dummy_debug_msl ) ;
        }

        // init render window rendering objects
        if( wid == _rwid )
        {
            fe->configure<motor::graphics::framebuffer_object_t>( &pp_fb ) ;
            fe->configure< motor::graphics::state_object_t>( &_post_process_rs ) ;
            fe->configure< motor::graphics::state_object_t>( &_scene_final_rs ) ;

            fe->configure<motor::graphics::geometry_object>( _post_quad ) ;
            fe->configure<motor::graphics::msl_object>( _post_msl ) ;

            fe->configure<motor::graphics::geometry_object>( _dummy_geo ) ;
            fe->configure<motor::graphics::msl_object>( _dummy_render_msl ) ;
        }
    }

    // debug view does not use a post framebuffer
    if( wid == _dwid )
    {
        {
            fe->push( &_debug_rs ) ;
            {
                motor::graphics::gen4::backend::render_detail_t det ;
                fe->render( _dummy_debug_msl, det ) ;
            }
            fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
        }

        // do the primitive renderer
        {
            pr.prepare_for_rendering( fe ) ;
            fe->push( &_pr_rs ) ;
            pr.render( fe ) ;
            fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
        }
    }
    else if ( wid == _rwid )
    {
        // render scene
        {
            fe->use( &pp_fb ) ;
            {
                fe->push( &_scene_final_rs ) ;
                {
                    motor::graphics::gen4::backend::render_detail_t det ;
                    fe->render( _dummy_render_msl, det ) ;
                }
                fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
            }
            fe->unuse( motor::graphics::gen4::backend::unuse_type::framebuffer ) ;
        }

        // render post 
        {
            fe->push( &_post_process_rs ) ;
            motor::graphics::gen4::backend::render_detail_t det ;
            fe->render( _post_msl, det ) ;
            fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
        }
    }
    
}