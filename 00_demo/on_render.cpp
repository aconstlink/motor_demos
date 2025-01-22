
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
        }

        // init render window rendering objects
        if( wid == _rwid )
        {
            fe->configure<motor::graphics::framebuffer_object_t>( &pp_fb ) ;
            fe->configure< motor::graphics::state_object_t>( &rs ) ;
        }
        
    }

    // debug view does not use a post framebuffer
    if( wid == _dwid )
    {
        // do the primitive renderer
        {
            pr.prepare_for_rendering( fe ) ;
            //fe->push( &rs ) ;
            pr.render( fe ) ;
            //fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
        }
    }
    else if ( wid == _rwid )
    {
        fe->use( &pp_fb ) ;
        {
            fe->push( &rs ) ;
            fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
        }
        fe->unuse( motor::graphics::gen4::backend::unuse_type::framebuffer ) ;
    }
    
}