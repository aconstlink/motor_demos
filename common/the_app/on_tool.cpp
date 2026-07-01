
#include "the_app.h"
#include <motor/tool/util.hpp>

using namespace demos;

//******************************************************************************************************
bool_t the_app::on_tool(
    this_t::window_id_t const wid, motor::application::app::tool_data_ref_t td ) noexcept
{

    if( wid != _twid && !_need_tool_view ) return false;

    {
        _sm.on_tool();
    }

    auto slider_int_fn =
        [ & ]( char const * name, size_t & io_value, size_t const min, size_t const max )
    {
        int_t v = int_t( io_value );
        ImGui::SliderInt( name, &v, int_t( min ), int_t( max ) );
        io_value = size_t( v );
    };

    {
        motor::tool::time_info_t ti{ _sm.get_whole_duration() + 2000, _cur_time };

        if( ImGui::Begin( "Timeline" ) )
        {
            // the timeline stores some state, so it
            // is defined further above
            {
                tl.begin( ti );

                _sm.for_each_scene(
                    [ & ]( size_t const, demos::scene_manager::for_each_scene_info_in_t info )
                {
                    tl.place_marker( motor::tool::timeline::marker_position::bottom, info.start,
                        ImColor( 0, 100, 0, 255 ) );
                    tl.place_marker( motor::tool::timeline::marker_position::bottom, info.end,
                        ImColor( 100, 0, 0, 255 ) );
                } );

                tl.end();
                _cur_time = ti.cur_milli;
            }

            // the player_controller stores some state,
            // so it is defined further above
            {
                auto const s = pc.do_tool( "Player", _space_bar_pressed );
                if( s == motor::tool::player_controller_t::player_state::stop )
                {
                    _cur_time = 0;
                    _proceed_time = false;
                }
                else if( s == motor::tool::player_controller_t::player_state::play )
                {
                    _proceed_time = true;
                    if( ti.cur_milli >= ti.max_milli )
                    {
                        _cur_time = 0;
                    }
                }
                else if( s == motor::tool::player_controller_t::player_state::pause )
                {
                    _proceed_time = false;
                }

                if( _cur_time > ti.max_milli )
                {
                    _cur_time = ti.max_milli;
                    pc.set_stop();
                }
                _space_bar_pressed = false;
            }
        }
        ImGui::End();
    }

    // audio stuff
    {
        if( ImGui::Begin( "Capture Audio" ) )
        {
            // print wave form
            {
                auto const mm = _co.minmax();
                ImGui::PlotLines( "Samples", _aanl.captured_samples.data(),
                    (int)_aanl.captured_samples.size(), 0, 0, mm.x(), mm.y(),
                    ImVec2( ImGui::GetWindowWidth(), 100.0f ) );
            }

            // print frequencies captured
            {
                ImGui::PlotHistogram( "Frequencies Captured", _aanl.captured_frequencies.data(),
                    (int)_aanl.captured_frequencies.size() >> 2, 0, 0, 0.0f, 1.0f,
                    ImVec2( ImGui::GetWindowWidth(), 100.0f ) );
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
                ImGui::Checkbox( "Kick", &_aanl.asys.is_kick );
                ImGui::SameLine();
                ImGui::Checkbox( "Mid", &_aanl.asys.is_lowm );
                ImGui::SameLine();
            }

            {
                ImGui::VSliderFloat(
                    "Kick Value", ImVec2( 18, 160 ), &( _aanl.asys.kick ), 0.0f, 1.0f );
                ImGui::SameLine();
                ImGui::VSliderFloat(
                    "Midl Value", ImVec2( 18, 160 ), &( _aanl.asys.midl ), 0.0f, 1.0f );
            }
        }
        ImGui::End();
    }

    if( ImGui::Begin( "Controls Info" ) )
    {
    }
    ImGui::End();

    return true;
}