
#include "main.h"

using namespace demos ;

//******************************************************************************************************
void_t the_app::on_render( this_t::window_id_t const wid, motor::graphics::gen4::frontend_ptr_t fe,
    motor::application::app::render_data_in_t rd ) noexcept
{
    if ( rd.first_frame )
    {
        // init debug window rendering objects
        if( wid == _dwid )
        {
            pr.configure( fe ) ;
            fe->configure< motor::graphics::state_object_t>( &_pr_rs ) ;
            fe->configure< motor::graphics::state_object_t>( &_dv_rs ) ;
        }

        // init render window rendering objects
        if( wid == _rwid )
        {
            fe->configure< motor::graphics::state_object_t>( &_scene_final_rs ) ;
            fe->configure<motor::graphics::framebuffer_object_t>( &pp_fb0 ) ;
            fe->configure<motor::graphics::framebuffer_object_t>( &pp_fb1 ) ;
            fe->configure< motor::graphics::state_object_t>( &_post_process_rs ) ;
            fe->configure<motor::graphics::geometry_object>( _post_quad ) ;
            fe->configure<motor::graphics::msl_object>( _post_msl ) ;
        }
    }

    // debug view does not use a post framebuffer
    if( wid == _dwid )
    {
        {
            fe->push( &_dv_rs ) ;
            for ( auto & s : _scenes )
            {
                s.s->on_render_debug( rd.first_frame, fe ) ;
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
        #if 0
        if( _gbuffer_sel_changed )
        {
            // need variable set update for msl object
            fe->update( _post_msl, 0 ) ;
            _gbuffer_sel_changed = false ;
        }
        #endif

        // render and post
        {
            size_t cur_scene = size_t( -1 ) ;
            size_t nxt_scene = size_t( -1 ) ;

            {
                while ( ++cur_scene < _scenes.size() )
                {
                    if( _scenes[ cur_scene ].s->is_in_time_range( _cur_time ) ) break ;
                }

                if ( ( cur_scene + 1 ) < _scenes.size() &&
                    _scenes[ cur_scene + 1 ].s->is_in_time_range( _cur_time ) )
                    nxt_scene = cur_scene + 1 ;
            }

            // #1 : Render the actual scene in the GBuffers
            {
                if ( cur_scene != size_t( -1 ) )
                {
                    auto * s = _scenes[ cur_scene ].s ;

                    fe->use( &pp_fb0 ) ;
                    fe->push( &_scene_final_rs ) ;
                    s->on_render_final( rd.first_frame, fe ) ;
                    fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
                    fe->unuse( motor::graphics::gen4::backend::unuse_type::framebuffer ) ;
                }

                if ( nxt_scene != size_t( -1 ) )
                {
                    auto * s = _scenes[ cur_scene ].s ;

                    fe->use( &pp_fb1 ) ;
                    fe->push( &_scene_final_rs ) ;
                    s->on_render_final( rd.first_frame, fe ) ;
                    fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
                    fe->unuse( motor::graphics::gen4::backend::unuse_type::framebuffer ) ;
                }
            }

            // #2 : TO-DO : do post processing for rendered scenes
            {
            }

            // #3 : Combine the render posts
            {
                bool_t const need_mix = cur_scene != size_t(-1) && nxt_scene != size_t(-1) ;

                if( !need_mix )
                {
                    fe->push( &_post_process_rs ) ;
                    motor::graphics::gen4::backend::render_detail_t det ;
                    fe->render( _post_msl, det ) ;
                    fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
                }
                else
                {
                    // do the mix
                }
                
            }
        }
    }
}