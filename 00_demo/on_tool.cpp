
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

    {
        for( auto * s : _scenes ) 
        {
            s->on_tool() ;
        }
    }

    if ( ImGui::Begin( "Keyboard Info" ) )
    {
        {
            ImGui::Text("F3 will open a gen4_auto window") ;
            ImGui::Text("F4 will make the render window fullscreen") ;
            ImGui::Separator() ;

            ImGui::Text("c will capture a camera pos and lookat coordinate") ;
            ImGui::Text("c + ctrl will clear the captured camera data") ;
            ImGui::Text("p will print the camera storage per scene in a file") ;
        }
        #if 0
        {
            int used_cam = int_t( _final_cam_idx ) ;
            ImGui::SliderInt( "Choose Render Camera", &used_cam, 1, int( this_t::get_num_cams() - 1 ) ) ;
            _final_cam_idx = std::min( size_t( used_cam ), size_t( this_t::get_num_cams() - 1 ) ) ;
        }
        #endif
    }
    ImGui::End() ;

    if ( ImGui::Begin( "Render Window" ) )
    {
        int_t sel = int_t(_gbuffer_selection) ;
        if( ImGui::SliderInt( "Select G-Buffer##maintool", &sel, 0, 3 ) )
        {
            if( sel != int_t(_gbuffer_selection))
            {
                _gbuffer_sel_changed = true ;
                _gbuffer_selection = size_t( sel ) ;
            }
        }
        
    }
    ImGui::End() ;

    
    if ( ImGui::Begin( "Camera Window" ) )
    {
        if( ImGui::Checkbox( "Use Free Camera", &_use_free_camera ) )
        {
        }


        {
            ImGui::Text("Place Free Camera to keyframe control point") ;
            int num_cams = (int_t)this_t::get_current_scene()->camera_manager().get_num_cameras() ;
            int sel_cam = (int_t)this_t::get_current_scene()->camera_manager().get_final_camera_idx() ;
            ImGui::SliderInt( "Which Camera##maintool", &sel_cam, 0, num_cams-1 ) ;

            demos::camera_data cd ;
            if( this_t::get_current_scene()->camera_manager().get_camera_data( size_t(sel_cam), cd ) )
            {
                static bool_t lock_selection = true ;
                static int_t sel_kf = 0 ;
                bool_t set_lookat = false ;
                bool_t set_now = false ;

                ImGui::Checkbox( "Lock Selection##selectionforfreecameratokeyframe", &lock_selection ) ;
                ImGui::SameLine() ;
                if( ImGui::Checkbox( "Place now##setfreecameranowtokeyframe", &set_now ) )
                {
                    set_now = true ;
                }

                // position
                {
                    int_t const num_keyframes = (int_t)cd.kf_pos.get_num_keyframes() ;
                    
                    if ( ImGui::SliderInt( "Position##KeyframeFreeCamera", &sel_kf, 0, num_keyframes-1 ) || set_now )
                    {
                        _camera.translate_to( cd.kf_pos.get_spline().get_control_point( size_t(sel_kf) ).p ) ;
                        set_lookat = true ;
                    }
                }

                // lookat
                {
                    int_t const num_keyframes = (int_t)cd.kf_lookat.get_num_keyframes() ;
                    static int_t sel_kf2 = 0 ;
                    sel_kf2 = lock_selection ? std::min( sel_kf, num_keyframes ) : sel_kf2 ;
                    if ( ImGui::SliderInt( "Lookat##KeyframeFreeCamera", &sel_kf2, 0, num_keyframes - 1 ) )
                    {
                        
                    }

                    if( set_lookat )
                    {
                        auto const lookat = cd.kf_lookat.get_spline().get_control_point( size_t( sel_kf2 ) ).p ;
                        _camera.look_at( _camera.get_position(), motor::math::vec3f_t( 0.0f, 1.0f, 0.0f ), lookat ) ;
                    }
                }
            }

        }

        #if 0
        {
            int used_cam = int_t( _cam_idx ) ;
            ImGui::SliderInt( "Choose Debug Camera", &used_cam, 0, int( this_t::get_num_cams() - 1 ) ) ;
            _cam_idx = std::min( size_t( used_cam ), this_t::get_num_cams() - 1 ) ;
        }

        {
            auto const cam_pos = _camera[ _cam_idx ].cam.get_position() ;
            float x = cam_pos.x() ;
            float y = cam_pos.y() ;
            ImGui::SliderFloat( "Cur Cam X", &x, -100.0f, 100.0f ) ;
            ImGui::SliderFloat( "Cur Cam Y", &y, -100.0f, 100.0f ) ;
            _camera[ _cam_idx ].cam.translate_to( motor::math::vec3f_t( x, y, cam_pos.z() ) ) ;
        }
        #endif
    }
    ImGui::End() ;

    
    if ( ImGui::Begin( "Global Settings" ) )
    {
        ImGui::Text("Primitive Renderer") ;
        _pr_rs.access_render_state( 0, [&]( motor::graphics::render_state_sets_ref_t rs )
        {
            bool_t is_depth_test = rs.depth_s.do_change && rs.depth_s.ss.do_activate ;
            if ( ImGui::Checkbox( "Depth Test Debug Elements", &is_depth_test ) )
            {
                if( is_depth_test )
                {
                    rs.depth_s.do_change = true ;
                    rs.depth_s.ss.do_activate = true ;
                }
                else
                {
                    rs.depth_s.do_change = true ;
                    rs.depth_s.ss.do_activate = false ;
                }
                return true ;
            }
            return false ;
        } ) ;

        ImGui::Text("Debug Window") ;
        _dv_rs.access_render_state( 0, [&] ( motor::graphics::render_state_sets_ref_t rs )
        {
            bool_t is_flag = rs.polygon_s.ss.fm == motor::graphics::fill_mode::line ;
            if( ImGui::Checkbox("Wireframe##DebugWindow", &is_flag) )
            {
                rs.polygon_s.ss.fm = is_flag ? motor::graphics::fill_mode::line : 
                    motor::graphics::fill_mode::fill ;

                return true ;
            }
            return false ;
        } ) ;
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

    // audio stuff
    {
        if ( ImGui::Begin( "Capture Audio" ) )
        {

            // print wave form
            {
                auto const mm = _co.minmax() ;
                ImGui::PlotLines( "Samples", _aanl.captured_samples.data(), (int) _aanl.captured_samples.size(), 0, 0, mm.x(), mm.y(), 
                    ImVec2( ImGui::GetWindowWidth(), 100.0f ) );
            }

            // print frequencies captured
            {
                ImGui::PlotHistogram( "Frequencies Captured", _aanl.captured_frequencies.data(),
                    (int) _aanl.captured_frequencies.size() >> 2, 0, 0, 0.0f, 1.0f, ImVec2( ImGui::GetWindowWidth(), 100.0f ) ) ;
            }

            #if 0
            // print frequencies captured 4th - quater size
            {
                ImGui::PlotHistogram( "Frequencies Captured 4", captured_frequencies.data(),
                    (int) ( captured_frequencies.size() >> 2 ), 0, 0, 0.0f, 1.0f, ImVec2( ImGui::GetWindowWidth(), 100.0f ) ) ;
            }

            // print frequencies average
            {
                ImGui::PlotHistogram( "Frequencies Average", captured_frequencies_avg.data(),
                    (int) captured_frequencies_avg.size(), 0, 0, 0.0f, 1.0f, ImVec2( ImGui::GetWindowWidth(), 100.0f ) ) ;
            }

            // print frequencies variance
            {
                ImGui::PlotHistogram( "Frequencies Variance", captured_frequencies_var.data(),
                    (int) captured_frequencies_var.size(), 0, 0, 0.0f, 1.0f, ImVec2( ImGui::GetWindowWidth(), 100.0f ) ) ;
            }

            #endif
            {
                ImGui::Checkbox( "Kick", &_aanl.asys.is_kick ) ; ImGui::SameLine() ;
                ImGui::Checkbox( "Mid", &_aanl.asys.is_lowm ) ; ImGui::SameLine() ;
            }

            {
                ImGui::VSliderFloat( "Kick Value", ImVec2( 18, 160 ), &( _aanl.asys.kick ), 0.0f, 1.0f ) ; ImGui::SameLine() ;
                ImGui::VSliderFloat( "Midl Value", ImVec2( 18, 160 ), &( _aanl.asys.midl ), 0.0f, 1.0f ) ;
            }

        }
        ImGui::End() ;
    }

    return true ; 
}