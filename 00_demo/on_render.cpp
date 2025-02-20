
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
            fe->configure<motor::graphics::msl_object>( _post_xfade_msl ) ;
        }
    }

    /////////////////////////////////////////////////////////////////////
    // handle render scene states
    /////////////////////////////////////////////////////////////////////
    {
        for ( size_t i = 0; i < _scenes.size(); ++i )
        {
            auto & sd = _scenes[ i ] ;

            // debug window init
            if ( wid == _dwid )
            {
                if ( sd.ss_dbg == demos::scene_state::init )
                {
                    // could put time constraint here. So render object init
                    // is carried out later. Time constraint could be placed near the
                    // scene time range.
                    sd.s->on_render_init( demos::iscene::render_mode::tool, fe ) ;
                    sd.ss_dbg = demos::scene_state::render_init ;
                }

                // at the moment, no async render init, so just move on.
                else if ( sd.ss_dbg == demos::scene_state::render_init )
                {
                    sd.ss_dbg = demos::scene_state::ready ;
                }

                else if ( sd.ss_dbg == demos::scene_state::render_deinit_trigger )
                {
                    sd.s->on_render_deinit( demos::iscene::render_mode::tool, fe ) ;
                    sd.ss_dbg = demos::scene_state::render_deinit ;
                }
            }
            // prod window init
            else if ( wid == _rwid )
            {
                if ( sd.ss_prod == demos::scene_state::init )
                {
                    // could put time constraint here. So render object init
                    // is carried out later. Time constraint could be placed near the
                    // scene time range.
                    sd.s->on_render_init( demos::iscene::render_mode::production, fe ) ;
                    sd.ss_prod = demos::scene_state::render_init ;
                }

                // at the moment, no async render init, so just move on.
                else if ( sd.ss_prod == demos::scene_state::render_init )
                {
                    sd.ss_prod = demos::scene_state::ready ;
                }

                else if ( sd.ss_prod == demos::scene_state::render_deinit_trigger )
                {
                    sd.s->on_render_deinit( demos::iscene::render_mode::production, fe ) ;
                    sd.ss_prod = demos::scene_state::render_deinit ;
                }
            }
        }
    }

    // debug view does not use a post framebuffer
    if( wid == _dwid )
    {
        auto const tmp = this_t::current_scene_idx() ;

        size_t const cur_scene = tmp.first ;
        size_t const nxt_scene = tmp.second ;
        
        if ( cur_scene != size_t( -1 ) )
        {
            auto & s = _scenes[ cur_scene ] ;

            if( s.ss_dbg == demos::scene_state::ready )
            {
                fe->push( &_dv_rs ) ;
                s.s->on_render_debug( fe ) ;
                fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
            }
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
        // deinit render objects
        {
            for ( size_t i = 0; i < _scenes.size(); ++i )
            {
                auto & sd = _scenes[ i ] ;
                if ( sd.ss_prod == demos::scene_state::render_deinit_trigger )
                {
                    sd.s->on_render_deinit( demos::iscene::render_mode::production, fe ) ;
                    sd.ss_prod = demos::scene_state::render_deinit ;
                }
            }
        }

        // render and post
        {
            auto const tmp = this_t::current_scene_idx() ;

            size_t const cur_scene = tmp.first ;
            size_t const nxt_scene = tmp.second ;

            // #1 : Render the actual scene in the GBuffers
            {
                if ( cur_scene != size_t( -1 ) )
                {
                    auto & s = _scenes[ cur_scene ] ;

                    if ( s.ss_dbg == demos::scene_state::ready )
                    {
                        fe->use( &pp_fb0 ) ;
                        fe->push( &_scene_final_rs ) ;
                        s.s->on_render_final( fe ) ;
                        fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
                        fe->unuse( motor::graphics::gen4::backend::unuse_type::framebuffer ) ;
                    }
                }

                if ( nxt_scene != size_t( -1 ) )
                {
                    auto & s = _scenes[ nxt_scene ] ;

                    if ( s.ss_dbg == demos::scene_state::ready )
                    {
                        fe->use( &pp_fb1 ) ;
                        fe->push( &_scene_final_rs ) ;
                        s.s->on_render_final( fe ) ;
                        fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
                        fe->unuse( motor::graphics::gen4::backend::unuse_type::framebuffer ) ;
                    }
                }
            }

            // #2 : TO-DO : do post processing for rendered scenes
            {
            }

            // #3 : Combine the render posts
            {
                float_t overlap = 0.0f ;

                fe->push( &_post_process_rs ) ;
                if( this_t::is_in_transition( overlap ) )
                {
                    auto & vss = _post_xfade_msl->borrow_varibale_sets() ;
                    for ( size_t i = 0; i < vss.size(); ++i )
                    {
                        auto * vs = vss[ i ] ;
                        auto * d = vs->data_variable<float_t>( "u_overlap" ) ;
                        d->set( overlap ) ;
                    }
                    motor::graphics::gen4::backend::render_detail_t det ;
                    fe->render( _post_xfade_msl, det ) ;
                }
                else
                {
                    motor::graphics::gen4::backend::render_detail_t det ;
                    fe->render( _post_msl, det ) ;
                }
                fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
                
            }
        }
    }
}