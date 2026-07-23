#pragma once

#include "../common/scene/iscene.h"
#include "../common/camera_collector.hpp"
#include "../common/msl_component_collector.hpp"

#include <motor/scene/node/logic_group.h>
#include <motor/scene/component/name_component.hpp>
#include <motor/scene/component/trafo3d_component.h>
#include <motor/scene/visitor/trafo_visitor.h>
#include <motor/scene/visitor/graphics/render_visitor.h>
#include <motor/scene/visitor/variable_update_visitor.h>
#include <motor/scene/component/graphics/render_settings_component.hpp>
#include <motor/scene/visitor/graphics/add_shader_to_set_visitor.hpp>

#include <motor/graphics/state/render_states.h>

#include <motor/tool/imgui/node_kit/imgui_node_visitor.h>

#include <motor/format/global.h>
#include <motor/log/global.h>
#include <motor/memory/global.h>
#include <motor/concurrent/global.h>

namespace demos
{

// loading screen
class intro_scene : public iscene
{
    motor_this_typedefs( intro_scene );

  private: // wire

    using os_float_t = motor::wire::output_slot< float_t >;
    using os_trafo_t = motor::wire::output_slot< motor::math::m3d::trafof_t >;

    os_float_t * _time = nullptr;
    os_trafo_t * _scale_os = nullptr;

    motor::wire::time_node_mtr_t _time_node = nullptr; // from the importer
    motor::wire::inode_mtr_t _merger = nullptr;        // from the importer

    // store all the wire nodes for easier destruction.
    motor::vector< motor::wire::inode_mtr_t > _node_dump;

  private: // graphcis/scene graph

    motor::graphics::state_object_mtr_t _root_so = nullptr;
    motor::graphics::state_object_mtr_t _final_so = nullptr;
    motor::scene::node_mtr_t _root = nullptr;

    motor::scene::node_mtr_t _selected = nullptr;

  private: // camera

    size_t _cam_id = size_t( -1 );
    motor::vector< std::pair< motor::string_t, motor::gfx::generic_camera_mtr_t > > _cameras;

    demos::camera_collector_t _cc;

    struct camera_sequence_item
    {
        motor::math::time_ms_t start;
        motor::math::time_ms_t end;
        motor::gfx::generic_camera_mtr_t cam;
    };

    motor::vector< camera_sequence_item > _cs;

  private: // shadow mapping

    motor::gfx::generic_camera_mtr_t _sun_cam = nullptr;
    motor::graphics::framebuffer_object_mtr_t _shadow_fb = nullptr;
    motor::graphics::state_object_mtr_t _shadow_so = nullptr;

  private: // io

    motor::io::monitor_mtr_t _gltf_mon = nullptr;

    // borrowed
    motor::io::database_mtr_t _db = nullptr;

  private: // general variables

    // 0: can start parallel work
    // 1: is in parallel work/has to wait for result
    // 2: can do serial work after parallel is done
    size_t _update_sync = 0;

    motor::scene::node_mtr_t _selected_node = nullptr;

  public:

    intro_scene( motor::string_cref_t name ) noexcept : iscene( name, { 4000, 2000 } ) {}
    intro_scene( intro_scene const & ) = delete;
    intro_scene( intro_scene && rhv ) noexcept
        : iscene( std::move( rhv ) ), _gltf_mon( motor::move( rhv._gltf_mon ) )
    {
    }
    virtual ~intro_scene( void_t ) noexcept
    {
        this_t::release_all_objects();
        motor::release( motor::move( _gltf_mon ) );
    }

  public:

    //************************************************************************************
    virtual void_t on_init( motor::io::database_ptr_t db ) noexcept
    {
        _db = db;
        _gltf_mon = motor::shared( motor::io::monitor_t(), "gltf file monitor" );

        {
            _time = motor::shared( os_float_t( 0.0f ) );
            _scale_os = motor::shared( os_trafo_t() );
        }

        {
            motor::graphics::state_object_t so =
                motor::graphics::state_object_t( "scene.00.final_render_states" );

            {
                motor::graphics::render_state_sets_t rss;
                rss.depth_s.do_change = true;
                rss.depth_s.ss.do_activate = true;
                rss.depth_s.ss.do_depth_write = true;
                rss.polygon_s.do_change = true;
                rss.polygon_s.ss.do_activate = true;
                rss.polygon_s.ss.fm = motor::graphics::fill_mode::fill;
                rss.polygon_s.ss.ff = motor::graphics::front_face::counter_clock_wise;
                rss.polygon_s.ss.cm = motor::graphics::cull_mode::back;
                rss.clear_s.do_change = true;
                rss.clear_s.ss.clear_color = motor::math::vec4f_t( 0.0f, 0.0f, 0.0f, 1.0f );
                rss.clear_s.ss.do_activate = true;
                rss.clear_s.ss.do_color_clear = true;
                rss.clear_s.ss.do_depth_clear = true;
                rss.view_s.do_change = true;
                rss.view_s.ss.do_activate = false;
                rss.view_s.ss.vp = motor::math::vec4ui_t( 0, 0, 500, 500 );
                so.add_render_state_set( rss );
            }

            _final_so = motor::shared( motor::graphics::state_object_t( std::move( so ) ) );
        }

        {
            motor::graphics::state_object_t so =
                motor::graphics::state_object_t( "scene.00.root_render_states" );

            {
                motor::graphics::render_state_sets_t rss;
                rss.depth_s.do_change = true;
                rss.depth_s.ss.do_activate = true;
                rss.depth_s.ss.do_depth_write = true;
                rss.polygon_s.do_change = true;
                rss.polygon_s.ss.do_activate = true;
                rss.polygon_s.ss.fm = motor::graphics::fill_mode::fill;
                rss.polygon_s.ss.ff = motor::graphics::front_face::counter_clock_wise;
                rss.polygon_s.ss.cm = motor::graphics::cull_mode::back;
                rss.clear_s.do_change = true;
                rss.clear_s.ss.clear_color = motor::math::vec4f_t( 0.5f, 0.5f, 0.5f, 1.0f );
                rss.clear_s.ss.do_activate = true;
                rss.clear_s.ss.do_color_clear = true;
                rss.clear_s.ss.do_depth_clear = true;
                rss.view_s.do_change = true;
                rss.view_s.ss.do_activate = false;
                rss.view_s.ss.vp = motor::math::vec4ui_t( 0, 0, 500, 500 );
                so.add_render_state_set( rss );
            }

            _root_so = motor::shared( motor::graphics::state_object_t( std::move( so ) ) );
        }

        {
            motor::graphics::state_object_t so =
                motor::graphics::state_object_t( "scene.00.shadow_render_states" );

            {
                motor::graphics::render_state_sets_t rss;
                rss.depth_s.do_change = true;
                rss.depth_s.ss.do_activate = true;
                rss.depth_s.ss.do_depth_write = true;
                rss.polygon_s.do_change = true;
                rss.polygon_s.ss.do_activate = true;
                rss.polygon_s.ss.fm = motor::graphics::fill_mode::fill;
                rss.polygon_s.ss.ff = motor::graphics::front_face::counter_clock_wise;
                rss.polygon_s.ss.cm = motor::graphics::cull_mode::back;
                rss.clear_s.do_change = true;
                rss.clear_s.ss.clear_color = motor::math::vec4f_t( 0.0f, 0.0f, 0.0f, 1.0f );
                rss.clear_s.ss.do_activate = true;
                rss.clear_s.ss.do_color_clear = true;
                rss.clear_s.ss.do_depth_clear = true;
                rss.view_s.do_change = true;
                rss.view_s.ss.do_activate = true;
                rss.view_s.ss.vp = motor::math::vec4ui_t( 0, 0, 1000, 1000 );
                so.add_render_state_set( rss );
            }

            _shadow_so = motor::shared( motor::graphics::state_object_t( std::move( so ) ) );
        }

        // load scene
        {
            auto const loc = motor::io::location_t( "0_intro_scene.scene.gltf" );
            this_t::init_scene_and_components( loc );
            _db->attach( loc, motor::share( _gltf_mon ) );
        }

        // load post shaders
        {
            motor::string_t shd;

            auto shader_b =
                _db->load( motor::io::location_t( "shaders.post_process.map_to_screen.msl" ) );

            {
                shader_b.wait_for_operation(
                    [ & ]( char_cptr_t data, size_t const sib, motor::io::result const )
                {
                    motor::log::global_t::status( "********************************" );
                    motor::log::global_t::status(
                        "loaded shader " + motor::from_std( std::to_string( sib ) ) + " bytes" );

                    shd = motor::string_t( data, sib );
                } );

                // motor::scene::add_shader_to_set_visitor_t v( 2, "shadow", shd );
                // motor::scene::node_t::traverser( _root ).apply( &v );
            }
        }
    }

    //**************************************************************************************
    void_t init_scene_and_components( motor::io::location_cref_t loc ) noexcept
    {
        // init scene tree
        {
            motor::scene::logic_group_t root;
            root.add_component( motor::shared( motor::scene::name_component_t( "intro scene" ) ) );

            // add imported scene
            {
                motor::scene::node_mtr_t imported_node = nullptr;

                auto rs = motor::shared( motor::scene::logic_group_t() );

// for now, no render states in the scene graph.
// the base render states are set before the particular
// scene is rendered.
#if 0
                {
                    auto rsc =
                        motor::scene::render_settings_component_t( motor::share( _root_so ) );

                    rs->add_component( motor::shared( std::move( rsc ) ) );

                    rs->add_component(
                        motor::shared( motor::scene::name_component_t( "Render Settings" ) ) );
                }
#endif

                // make importer ready
                {
                    motor::format::module_registry_mtr_t mod_reg =
                        motor::format::global::register_default_modules(
                            motor::shared( motor::format::module_registry_t(), "mod registry" ) );

                    // import the gltf asset.
                    {
                        motor::property::property_sheet_t ps;
                        ps.add_property< motor::string_t >( "base_name", this_t::name() );

                        auto item =
                            mod_reg->import_from( loc, _db, motor::shared( std::move( ps ) ) );

                        auto * ret_item = item.get();

                        if( auto * scene_item =
                                dynamic_cast< motor::format::scene_item_ptr_t >( ret_item );
                            scene_item != nullptr )
                        {
                            imported_node = motor::move( scene_item->root );

                            motor::wire::release( motor::move( _time_node ) );
                            motor::wire::release( motor::move( _merger ) );

                            _time_node = motor::move( scene_item->start_node );
                            _merger = motor::move( scene_item->merger_node );

                            _time_node->borrow_time_is()->connect( motor::share( _time ) );

                            // camera sequence
                            {
                                for( auto & csi : _cs )
                                    motor::release( motor::move( csi.cam ) );

                                _cs.clear();

                                for( auto & i : scene_item->camera_sequence )
                                {
                                    this_t::camera_sequence_item csi;
                                    csi.cam = motor::move( i.cam );
                                    csi.start = i.start;
                                    csi.end = i.end;
                                    _cs.emplace_back( csi );
                                }
                            }
                        }
                        else
                        {
                            motor::log::global_t::critical( "Failed to load gltf file." );
                            std::exit( 1 );
                        }

                        ret_item->release();
                        motor::release( motor::move( ret_item ) );
                    }

                    motor::release( motor::move( mod_reg ) );
                }

                // test and scale whole imported tree with
                // only one trafo component.
                {
                    motor::math::m3d::trafof_t t;
                    t.set_scale( motor::math::vec3f_t( 1.0f ) );

                    motor::scene::trafo3d_component_t comp;
                    comp.set_trafo( t );

                    {
                        motor::wire::inputs_t inputs;
                        comp.inputs( inputs );
                        inputs.connect( "trafo", motor::share( _scale_os ) );
                    }

                    imported_node->add_component( motor::shared( std::move( comp ) ) );
                }
                rs->add_child( motor::move( imported_node ) );
                root.add_child( motor::move( rs ) );
            }

            motor::release( motor::move( _root ) );
            _root = motor::shared( std::move( root ) );
        }

        // reload cameras
        {
            this_t::release_cameras();

            motor::scene::node_t::traverser( _root ).apply( &_cc );
            auto cams = _cc.get_cameras();

            if( cams.size() > 0 )
            {
                size_t i = 0;
                _cameras.resize( cams.size() );
                for( auto & c : cams )
                {
                    _cameras[ i ].first = c.first;
                    _cameras[ i++ ].second = motor::share( c.second );
                }
                _cam_id = 0;
            }
            else
            {
                motor::log::global_t::error( "gltf file has no cameras." );
            }
        }

        // reconnect nodes
        {
            this_t::release_node_dump();

            auto t = motor::shared( motor::wire::funk_node_t(
                [ = ]( motor::wire::funk_node_ptr_t ) { this->_update_sync = 2; } ) );

            _node_dump.emplace_back( motor::share( t ) );
            _merger->then( motor::move( t ) );
        }
        motor::release( motor::move( _selected_node ) );

        // load all shaders
        {
            motor::string_t shd;

            auto default_shader =
                _db->load( motor::io::location_t( "0_intro_scene.shaders.default_final.msl" ) );

            auto shadow_shader =
                _db->load( motor::io::location_t( "0_intro_scene.shaders.shadow_pass.msl" ) );

            {
                default_shader.wait_for_operation(
                    [ & ]( char_cptr_t data, size_t const sib, motor::io::result const )
                {
                    motor::log::global_t::status( "********************************" );
                    motor::log::global_t::status(
                        "loaded shader " + motor::from_std( std::to_string( sib ) ) + " bytes" );

                    shd = motor::string_t( data, sib );
                } );

                motor::scene::add_shader_to_set_visitor_t v(
                    demos::to_id( demos::msl_id::final_id ), "prod", shd );
                motor::scene::node_t::traverser( _root ).apply( &v );
            }

            {
                shadow_shader.wait_for_operation(
                    [ & ]( char_cptr_t data, size_t const sib, motor::io::result const )
                {
                    motor::log::global_t::status( "********************************" );
                    motor::log::global_t::status(
                        "loaded shader " + motor::from_std( std::to_string( sib ) ) + " bytes" );

                    shd = motor::string_t( data, sib );
                } );

                motor::scene::add_shader_to_set_visitor_t v(
                    demos::to_id( demos::msl_id::shadow_id ), "shadow", shd );
                motor::scene::node_t::traverser( _root ).apply( &v );
            }
        }

        // collect msl_components just added and add variable
        {
            demos::msl_component_collector_t v( 1 );
            motor::scene::node_t::traverser( _root ).apply( &v );

            v.for_each( [ & ]( motor::scene::msl_component_mtr_t mslc )
            {
                auto * inputs = mslc->borrow_shader_inputs();

                auto is = motor::wire::input_slot< motor::math::vec4f_t >(
                    motor::math::vec4f_t( 1.0f, 0.1f, 0.1f, 1.0f ) );
                inputs->add( "color", motor::shared( std::move( is ) ) );
            } );
        }

        // get sun camera and create shadow framebuffer
        {
            for( auto & cam : _cameras )
            {
                if( cam.first != "sun_cam" ) continue;
                _sun_cam = motor::share( cam.second );
                break;
            }

            float_t const w = _sun_cam->get_dims().x();
            float_t const h = _sun_cam->get_dims().y();

            auto fb = motor::graphics::framebuffer_object_t( "scene.00.shadow_framebuffer" );
            fb.set_target( motor::graphics::color_target_type::rgba_uint_8, 1 )
                .set_target( motor::graphics::depth_stencil_target_type::depth32 )
                .resize( size_t( 1000 ), size_t( 1000 ) );

            motor::release( motor::move( _shadow_fb ) );
            _shadow_fb = motor::shared( std::move( fb ) );
        }
    }

    //************************************************************************************
    virtual void_t on_release( void_t ) noexcept
    {
        this_t::release_all_objects();
    }

    // start parallel work
    void_t on_update_stage1( demos::iscene::update_data_cref_t ud ) noexcept
    {
        _gltf_mon->for_each_and_swap(
            [ & ]( motor::io::location_cref_t loc, motor::io::monitor_t::notify const n )
        { this_t::init_scene_and_components( loc ); } );

        if( _update_sync != 0 ) return;
        _update_sync = 1;

        {
            float_t t = ud.relative_seconds;
            t = motor::math::fn< float_t >::mod( t, 2.0 );
            // t = motor::math::fn<float_t>::abs( t ) ;
            // t = 1.0f - t ;

            _time->set_and_exchange( t );
        }

        {
            motor::concurrent::global_t::schedule(
                _time_node->get_task(), motor::concurrent::schedule_type::pool );
        }
    }

    // do work after parallel work is done.
    void_t on_update_stage2( demos::iscene::update_data_cref_t ud ) noexcept
    {
        if( _update_sync != 2 ) return;
        _update_sync = 0;

        {
            motor::scene::variable_update_visitor_t v;
            motor::scene::node_t::traverser( _root ).apply( &v );
        }

        {
            motor::scene::trafo_visitor_t v;
            motor::scene::node_t::traverser( _root ).apply( &v );
        }
    }

    //************************************************************************************
    virtual void_t on_update( demos::iscene::update_data_cref_t ud ) noexcept
    {
        this_t::on_update_stage1( ud );
        this_t::on_update_stage2( ud );
    }

    //************************************************************************************
    virtual void_t on_resize(
        demos::window_type const, uint_t const width, uint_t const height ) noexcept
    {
    }

    //************************************************************************************
    virtual void_t on_graphics( demos::iscene::on_graphics_data_in_t ) noexcept {}

    //************************************************************************************
    virtual void_t on_render_init( demos::window_type const wt,
        motor::graphics::gen4::frontend_ptr_t fe,
        motor::graphics::gen4::frontend::fence_funk_t funk ) noexcept
    {
        if( wt == demos::window_type::debug )
        {
            fe->configure< motor::graphics::state_object_t >( _root_so );
        }

        if( wt == demos::window_type::production )
        {
            fe->configure< motor::graphics::state_object_t >( _final_so );

            fe->configure< motor::graphics::state_object_t >( _shadow_so );

            fe->configure< motor::graphics::framebuffer_object_t >( _shadow_fb );
        }
        fe->fence( funk );
    }

    //************************************************************************************
    virtual void_t on_render_deinit( demos::window_type const wt,
        motor::graphics::gen4::frontend_ptr_t fe,
        motor::graphics::gen4::frontend::fence_funk_t funk ) noexcept
    {
        if( wt == demos::window_type::debug )
        {
            fe->release< motor::graphics::state_object_t >( _root_so );
        }

        if( wt == demos::window_type::production )
        {
            fe->release< motor::graphics::state_object_t >( _final_so );
            fe->release< motor::graphics::state_object_t >( _shadow_so );
            fe->release< motor::graphics::framebuffer_object_t >( _shadow_fb );
        }

        fe->fence( funk );
    }

    //************************************************************************************
    virtual void_t on_render_debug(
        size_t const wid, motor::graphics::gen4::frontend_ptr_t fe ) noexcept
    {
        if( _cam_id != size_t( -1 ) )
        {
            fe->push( _root_so );
            motor::gfx::generic_camera_mtr_t cam = _cameras[ _cam_id ].second;
            // cam->set_dims( 1000.0f, 1000.0f, 1.0f, 1000.0f) ;
            motor::scene::render_visitor_t vis(
                demos::to_id( demos::msl_id::default_id ), fe, cam );
            motor::scene::node_t::traverser( _root ).apply( &vis );
            fe->pop( motor::graphics::gen4::backend::pop_type::render_state );
        }
    }

    //************************************************************************************
    virtual void_t on_render_final_offscreen(
        size_t const, motor::graphics::gen4::frontend_ptr_t fe ) noexcept
    {
        // make shadow pass
        {
            fe->use( _shadow_fb );
            fe->push( _shadow_so );
            motor::gfx::generic_camera_mtr_t cam = _sun_cam;
            motor::scene::render_visitor_t vis( demos::to_id( demos::msl_id::shadow_id ), fe, cam );
            motor::scene::node_t::traverser( _root ).apply( &vis );
            fe->pop( motor::graphics::gen4::backend::pop_type::render_state );
            fe->unuse( motor::graphics::gen4::backend::unuse_type::framebuffer );
        }
    }

    //************************************************************************************
    virtual void_t on_render_final(
        size_t const wid, motor::graphics::gen4::frontend_ptr_t fe ) noexcept
    {
        if( _cam_id != size_t( -1 ) )
        {
            fe->push( _final_so );

            {
                motor::gfx::generic_camera_mtr_t cam = _cameras[ _cam_id ].second;
                // cam->set_dims( 1000.0f, 1000.0f, 1.0f, 1000.0f) ;
                motor::scene::render_visitor_t vis(
                    demos::to_id( demos::msl_id::final_id ), fe, cam );
                vis.set_light_direction( motor::math::vec3f_t( 0.0f, 1.0f, 0.0f ) );

                motor::scene::node_t::traverser( _root ).apply( &vis );
            }

            fe->pop( motor::graphics::gen4::backend::pop_type::render_state );
        }
    }

    //************************************************************************************
    virtual void_t on_tool( void_t ) noexcept
    {
        // SECTION: cameras
        {
            auto cams = _cc.get_cameras();

            if( cams.size() > 0 )
            {
                size_t i = 0;
                static ImGuiComboFlags flags = 0;
                motor::vector< char const * > items( cams.size() );
                for( auto const & e : cams )
                {
                    items[ i++ ] = e.first.c_str();
                }

                motor::string_t combo_name = "Scene Camera##" + this_t::name();

                int item_selected_idx = _cam_id != size_t( -1 ) ? int_t( _cam_id ) : 0;
                const char * combo_preview_value = items[ item_selected_idx ];
                if( ImGui::BeginCombo( combo_name.c_str(), combo_preview_value, flags ) )
                {
                    for( int n = 0; n < items.size(); n++ )
                    {
                        const bool is_selected = ( item_selected_idx == n );
                        if( ImGui::Selectable( items[ n ], is_selected ) ) item_selected_idx = n;

                        // Set the initial focus when opening the combo (scrolling +
                        // keyboard navigation focus)
                        if( is_selected ) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();

                    _cam_id = size_t( item_selected_idx );
                }
            }
        }

        // SECION: Scene Graph
        {
            {
                motor::tool::imgui_node_visitor_t v( motor::move( _selected ) );
                motor::scene::node_t::traverser( _root ).apply( &v );
                _selected = v.move_selected();
            }
        }
    }

  private:

    void_t release_cameras( void_t ) noexcept
    {
        _cc.release();
        for( auto & cam : _cameras )
        {
            motor::release( motor::move( cam.second ) );
        }
        _cameras.clear();

        for( auto & csi : _cs )
            motor::release( motor::move( csi.cam ) );
        _cs.clear();
    }

    void_t release_node_dump( void_t ) noexcept
    {
        for( auto * ptr : _node_dump )
        {
            ptr->disconnect();
            motor::release( motor::move( ptr ) );
        }
        _node_dump.clear();
    }

    void_t release_all_objects( void_t ) noexcept
    {
        motor::release( motor::move( _gltf_mon ) );

        motor::wire::release( motor::move( _time ) );
        motor::wire::release( motor::move( _scale_os ) );
        motor::wire::release( motor::move( _time_node ) );
        motor::wire::release( motor::move( _merger ) );

        motor::release( motor::move( _root_so ) );
        motor::release( motor::move( _final_so ) );
        motor::release( motor::move( _shadow_so ) );

        motor::release( motor::move( _root ) );
        motor::release( motor::move( _selected ) );

        this_t::release_cameras();
        motor::release( motor::move( _selected_node ) );

        this_t::release_node_dump();

        motor::release( motor::move( _sun_cam ) );
        motor::release( motor::move( _shadow_fb ) );
    }
};
motor_typedef( intro_scene );
} // namespace demos