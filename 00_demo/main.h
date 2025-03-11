
#include "app_camera.h"

#include <motor/platform/global.h>

#include <motor/tool/imgui/timeline.h>
#include <motor/tool/imgui/player_controller.h>

#include <motor/controls/types/ascii_keyboard.hpp>
#include <motor/controls/types/three_mouse.hpp>

#include <motor/graphics/object/framebuffer_object.h>

#include <motor/gfx/primitive/primitive_render_3d.h>
#include <motor/gfx/camera/generic_camera.h>

#include "../common/audio_analysis.h"
#include "../common/iscene.h"

#include <motor/math/utility/fn.hpp>
#include <motor/math/utility/angle.hpp>

#include <motor/math/quaternion/quaternion4.hpp>
#include <motor/math/utility/time.hpp>

#include <motor/std/vector>
#include <motor/log/global.h>
#include <motor/memory/global.h>
#include <motor/concurrent/global.h>

#include <future>


namespace demos
{
    using namespace motor::core::types ;

    // per scene
    enum class scene_state
    {
        raw,
        in_transit,
        init,
        ready,
        num_states
    };

    // per scene and per window
    // possible state transitions
    // raw -> init_triggered -> init -> ready
    // ready/init -> deinit_triggered -> raw
    enum class graphics_state
    {
        raw,
        init_triggered,
        init,
        ready,
        deinit_triggered,
        num_states
    };

    //******************************************************************
    class the_app : public motor::application::app
    {
        motor_this_typedefs( the_app ) ;

        // debug renderer
        motor::gfx::primitive_render_3d_t pr ;
        motor::graphics::state_object_t _pr_rs ;

        // debug view root render states
        motor::graphics::state_object_t _dv_rs ;

        motor::audio::capture_object_t _co ;
        demos::audio_analysis _aanl ;

        #if 0//PRODUCTION_MODE
        demos::demo_mode const _dm = demos::demo_mode::production ;
        #else
        demos::demo_mode const _dm = demos::demo_mode::tool ;
        #endif

        bool_t is_tool_mode( void_t ) const noexcept { return _dm == demos::demo_mode::tool ; }

    private: // file

        motor::io::monitor_mtr_t _mon = nullptr ; // motor::memory::create_ptr( motor::io::monitor_t() ) ;
        motor::io::database_mtr_t _db = nullptr ;

    private: // scenes

        // this function is only used in debug mode.
        // dont use for production
        demos::iscene_mtr_t get_current_scene( void_t ) noexcept ;

        bool_t call_for_current_scene( std::function< void_t ( demos::iscene_mtr_t ) > ) noexcept  ;

        // determined during on_update
        size_t _cur_scene_idx = size_t( -1 ) ;
        size_t _nxt_scene_idx = size_t( -1 ) ;

        std::pair< size_t, size_t > current_scene_idx( void_t ) const noexcept
        {
            return std::make_pair( _cur_scene_idx, _nxt_scene_idx ) ;
        }

        // it can be fase, if there is no scene is in range for
        // the current time point.
        bool_t has_current_scene( void_t ) const noexcept
        {
            return _cur_scene_idx != size_t(-1) ;
        }

        // called during on_update.
        void_t determine_scene_index( void_t ) noexcept
        {
            size_t cur_scene = size_t( -1 ) ;
            size_t nxt_scene = size_t( -1 ) ;

            // #0 : determine scene idx
            {
                while ( ++cur_scene < _scenes.size() )
                {
                    if ( _scenes[ cur_scene ].s->is_in_time_range( _cur_time ) ) break ;
                }
                if ( cur_scene == _scenes.size() ) cur_scene = size_t( -1 ) ;

                if ( ( cur_scene + 1 ) < _scenes.size() &&
                    _scenes[ cur_scene + 1 ].s->is_in_time_range( _cur_time ) )
                    nxt_scene = cur_scene + 1 ;
            }

            _cur_scene_idx = cur_scene ;
            _nxt_scene_idx = nxt_scene ;
        }

        // returns true if two scenes are transitioning and
        // if so, overlap is set. Otherwise false is returned.
        bool_t is_in_transition( float_t & overlap ) const noexcept
        {
            size_t cur_end = 0 ;
            size_t nxt_srt = 0 ;

            if ( _cur_scene_idx != size_t( -1 ) )
            {
                cur_end = _scenes[ _cur_scene_idx ].s->get_time_range().second ;
            }

            if( _nxt_scene_idx == size_t( -1 ) ) return false ;

            nxt_srt = _scenes[ _nxt_scene_idx ].s->get_time_range().first ;
            overlap = float_t(_cur_time - nxt_srt) / float_t( cur_end - nxt_srt ) ;
            return true ;
        }

        struct scene_data
        {
            // should the scene render in 
            // debug window?
            //bool_t render_in_debug = false ;

            demos::scene_state ss ;
            demos::graphics_state ss_dbg ;
            demos::graphics_state ss_prod ;
            demos::iscene_mtr_t s ;
        };
        motor::vector< scene_data > _scenes ;

        bool_t get_current_scene_data( this_t::scene_data & ) const noexcept ;
        bool_t access_current_scene_data( std::function< void_t ( scene_data & sd ) > ) noexcept ;
        

    private: // free camera 

        bool_t _use_free_camera = true ;
        motor::gfx::generic_camera_t _camera ;
        // this is only for user input interaction!
        helper::camera_controls::control_vector _cc ;

    private: // tool ui

        size_t _twid = size_t( -1 ) ;
        motor::tool::player_controller_t pc ;
        motor::tool::timeline_t tl = motor::tool::timeline_t( "my timeline" ) ;
        bool_t _space_bar_pressed = false ;

        // could be used in production
        bool_t _need_tool_view = false ;

        #if 1
        size_t _jump_to_scene = 0 ;
        #else
        size_t _jump_to_scene = size_t(-1) ;
        #endif

    private: // time control

        size_t _max_time_milli = motor::math::time::to_milli( 3, 11, 10 ) ;
        bool_t _proceed_time = false ;
        size_t _cur_time = 0 ;

    private: // final render stuff

        bool_t _gbuffer_sel_changed = false ;
        size_t _gbuffer_selection = 2 ;
        motor::graphics::state_object_t _scene_final_rs ;

    private: // post process

        size_t _rwid = size_t( -1 ) ;
        motor::math::vec4ui_t fb_dims = motor::math::vec4ui_t( 0, 0, 1920, 1080 ) ;
        motor::graphics::framebuffer_object_t pp_fb0 ; // the 1st scene is rendered to
        motor::graphics::framebuffer_object_t pp_fb1 ; // the 2nd scene is rendered to
        
        motor::graphics::msl_object_mtr_t _post_msl = nullptr ;
        motor::graphics::msl_object_mtr_t _post_xfade_msl = nullptr ;
        motor::graphics::geometry_object_mtr_t _post_quad = nullptr ;
        motor::graphics::state_object_t _post_process_rs ;

    private: // debug window
        
        size_t _dwid = size_t( -1 ) ;

    private:

        virtual void_t on_init( void_t ) noexcept ;
        virtual void_t on_event( window_id_t const wid,
            motor::application::window_message_listener::state_vector_cref_t sv ) noexcept ;
        virtual void_t on_device( device_data_in_t dd ) noexcept ;
        virtual void_t on_update( motor::application::app::update_data_in_t ) noexcept ;
        virtual void_t the_app::on_audio( motor::audio::frontend_ptr_t fptr, audio_data_in_t ad ) noexcept ;
        virtual void_t on_graphics( motor::application::app::graphics_data_in_t gd ) noexcept ;
        virtual void_t on_render( this_t::window_id_t const wid, motor::graphics::gen4::frontend_ptr_t fe,
            motor::application::app::render_data_in_t rd ) noexcept ;
        virtual bool_t on_tool( this_t::window_id_t const wid, motor::application::app::tool_data_ref_t td ) noexcept ;
        virtual void_t on_shutdown( void_t ) noexcept ;

        motor::string_t make_camera_data_file( void_t ) noexcept ;

    public:

        the_app( void_t ) noexcept ;
        the_app( this_rref_t ) noexcept ;
        virtual ~the_app( void_t ) noexcept ;
    } ;
}