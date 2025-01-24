
#include "main.h"

using namespace demos ;

//******************************************************************************************************
bool_t the_app::on_tool( this_t::window_id_t const wid, motor::application::app::tool_data_ref_t td ) noexcept 
{ 
    if( wid != _twid ) return false ;

    #if 0 
    // this only works if the scene is rendered in the 
    // same windows as the tool
    if ( ImGui::Begin( "LookiLooki" ) )
    {
        ImGui::Image( td.imgui->texture( "the_scene.0" ), ImGui::GetWindowSize() ) ;
    }
    ImGui::End() ;
    #endif

    if ( ImGui::Begin( "Camera Window" ) )
    {
        {
            int used_cam = int_t( _cam_idx ) ;
            ImGui::SliderInt( "Choose Debug Camera", &used_cam, 0, int( this_t::get_num_cams() - 1 ) ) ;
            _cam_idx = std::min( size_t( used_cam ), this_t::get_num_cams() - 1 ) ;
        }

        {
            int used_cam = int_t( _final_cam_idx ) ;
            ImGui::SliderInt( "Choose Render Camera", &used_cam, 1, int( this_t::get_num_cams() - 1 ) ) ;
            _final_cam_idx = std::min( size_t( used_cam ), size_t( this_t::get_num_cams() - 1 ) ) ;
        }

        {
            auto const cam_pos = _camera[ _cam_idx ].cam.get_position() ;
            float x = cam_pos.x() ;
            float y = cam_pos.y() ;
            ImGui::SliderFloat( "Cur Cam X", &x, -100.0f, 100.0f ) ;
            ImGui::SliderFloat( "Cur Cam Y", &y, -100.0f, 100.0f ) ;
            _camera[ _cam_idx ].cam.translate_to( motor::math::vec3f_t( x, y, cam_pos.z() ) ) ;

        }

        {
            bool_t ortho = _camera[ _cam_idx ].cam.is_orthographic() ;
            if( ImGui::Checkbox( "Orthographic", &ortho ) )
            {
                if( ortho ) _camera[ _cam_idx ].cam.orthographic() ;
                else _camera[ _cam_idx ].cam.perspective_fov() ;
            }
        }
    }
    ImGui::End() ;

    {
        motor::tool::time_info_t ti 
        { 
            _max_time_milli, cur_time 
        } ;

        if( ImGui::Begin("Timeline") )
        {
            // the timeline stores some state, so it 
            // is defined further above
            {
                tl.begin( ti ) ;
                tl.end() ;
                cur_time = ti.cur_milli ;
            }
        
            // the player_controller stores some state, 
            // so it is defined further above
            {
                auto const s = pc.do_tool( "Player" ) ;
                if ( s == motor::tool::player_controller_t::player_state::stop )
                {
                    cur_time = 0  ;
                    _proceed_time = false ;
                }
                else if ( s == motor::tool::player_controller_t::player_state::play )
                {
                    _proceed_time = true ;
                    if ( ti.cur_milli >= ti.max_milli )
                    {
                        cur_time = 0 ;
                    }
                }
                else if( s == motor::tool::player_controller_t::player_state::pause )
                {
                    _proceed_time = false ;
                }

                if ( cur_time > ti.max_milli )
                {
                    cur_time = ti.max_milli ;
                    pc.set_stop() ;
                }
            }
        }
        ImGui::End() ;
    }

    return true ; 
}