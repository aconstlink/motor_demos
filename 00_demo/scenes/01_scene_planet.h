#pragma once

#include "../../common/scene/iscene.h"
#include "../../common/camera_collector.hpp"

#include <motor/scene/node/logic_group.h>
#include <motor/scene/component/animation/animation_controller.h>
#include <motor/scene/component/name_component.hpp>
#include <motor/scene/component/graphics/render_settings_component.hpp>
#include <motor/scene/component/trafo3d_component.h>

#include <motor/scene/visitor/trafo_visitor.h>
#include <motor/scene/visitor/for_each_node_visitor.hpp>
#include <motor/scene/visitor/light_collector_visitor.hpp>
#include <motor/scene/visitor/graphics/render_visitor.h>
#include <motor/scene/visitor/graphics/light_pass_render_visitor.h>
#include <motor/scene/visitor/variable_update_visitor.hpp>
#include <motor/scene/visitor/graphics/connect_msl_slot_visitor.hpp>
#include <motor/scene/visitor/graphics/debug_graphics_visitor.hpp>

#include <motor/collide/3d/transform.hpp>
#include <motor/graphics/state/render_states.h>

#include <motor/tool/imgui/node_kit/imgui_node_visitor.h>
#include <motor/tool/imgui/imnodes_wire.h>

#include <motor/format/global.h>
#include <motor/log/global.h>
#include <motor/memory/global.h>
#include <motor/concurrent/global.h>

namespace demos::scene_01
{
class rotation_controller : public motor::scene::animation_controller
{
    using base_t = motor::scene::animation_controller;
    motor_this_typedefs( rotation_controller );

    using time_is_t = motor::wire::input_slot< float_t >;
    using rotation_os_t = motor::wire::output_slot< motor::math::quat4f_t >;

    // Borrowed from the wire node's slot sheets.
    time_is_t * _time_is = nullptr;
    rotation_os_t * _rotation_os = nullptr;

  public:

    rotation_controller( motor::math::vec3f_t axis, float_t const radians_per_second ) noexcept
    {
        motor::math::quat4f_t const identity( 1.0f, 0.0f, 0.0f, 0.0f );
        _time_is = motor::shared( time_is_t( 0.0f ) );
        _rotation_os = motor::shared( rotation_os_t( identity ) );

        auto node = motor::wire::funk_node_t( "rotation_controller",
            [ time = _time_is, rotation = _rotation_os, axis, radians_per_second, identity ](
                motor::wire::funk_node_ptr_t )
        {
            float_t const angle = time->get_value() * radians_per_second;
            rotation->set_and_exchange(
                axis.length2() > 0.0f ? motor::math::quat4f_t( angle, axis ) : identity );
        } );
        node.inputs().add( motor::share_unsafe( _time_is ) );
        node.outputs().add( motor::share_unsafe( _rotation_os ) );
        base_t::set_node( motor::shared( std::move( node ) ) );
    }

    rotation_controller( this_cref_t ) = delete;
    rotation_controller( this_rref_t rhv ) noexcept
        : base_t( std::move( rhv ) ), _time_is( motor::move( rhv._time_is ) ),
          _rotation_os( motor::move( rhv._rotation_os ) )
    {
    }

    time_is_t * borrow_time_is( void_t ) const noexcept
    {
        return _time_is;
    }
    rotation_os_t * borrow_rotation_os( void_t ) const noexcept
    {
        return _rotation_os;
    }
};
motor_typedef( rotation_controller );
} // namespace demos::scene_01

namespace demos
{
class scene_01_planet : public iscene
{
    motor_this_typedefs( scene_01_planet );

  private:

    using os_float_t = motor::wire::output_slot< float_t >;
    using os_trafo_t = motor::wire::output_slot< motor::math::m3d::trafof_t >;

    os_float_t * _time = nullptr;
    os_trafo_t * _scale_os = nullptr;

    motor::wire::time_node_mtr_t _time_node = nullptr; // from the importer
    motor::wire::inode_mtr_t _merger = nullptr;        // from the importer

    // 0: can start parallel work
    // 1: is in parallel work/has to wait for result
    // 2: can do serial work after parallel is done
    size_t _update_sync = 0;

    // store all the wire nodes for easier destruction.
    motor::vector< motor::wire::inode_mtr_t > _node_dump;

  private: // graphcis/scene graph

    motor::graphics::state_object_mtr_t _light_so = nullptr;
    motor::graphics::state_object_mtr_t _root_so = nullptr;
    motor::graphics::state_object_mtr_t _final_so = nullptr;
    motor::scene::node_mtr_t _root = nullptr;

    motor::scene::node_mtr_t _selected = nullptr;

    motor::collide::aabb3f_t _world_bounds;

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

    // this is where the depth of the light source is rendered to.
    // rendered from the light perspective.
    motor::graphics::framebuffer_object_mtr_t _shadow_depth_fb = nullptr;
    motor::graphics::state_object_mtr_t _shadow_depth_so = nullptr;

    // this is the scenes' shadow render target. this is where every lights'
    // shadow is rendered to.
    // rendered from the viewer perspecitve.
    motor::graphics::framebuffer_object_mtr_t _shadow_accum_fb = nullptr;
    motor::graphics::state_object_mtr_t _shadow_accum_so = nullptr;

  private:

    motor::vector< motor::scene::animation_controller_mtr_t > _controllers;

  private: // tool variables

    bool_t _render_normals = false;
    motor::gfx::primitive_render_3d_mtr_t _pr = nullptr;

    bool_t _draw_debug = false;

    motor::tool::imnodes_wire_t _imn_wire;

  private: // io

    motor::io::monitor_mtr_t _gltf_mon = nullptr;

    // borrowed
    motor::io::database_mtr_t _db = nullptr;

    // borrowed
    motor::gfx::msl_manager_mtr_t _mmgr = nullptr;
    motor::gfx::msl_manager_mtr_t _own_mmgr = nullptr;

    size_t _reg0 = size_t( -1 );
    size_t _reg1 = size_t( -1 );

  private: // generel

    motor::scene::node_mtr_t _selected_node = nullptr;

  private: // shadow variables

    motor::wire::os_float_mtr_t _bias = motor::shared( motor::wire::os_float_t( 0.1f ) );
    motor::wire::os_float_mtr_t _light_intensity = motor::shared( motor::wire::os_float_t( 1.0f ) );

  public:

    scene_01_planet( motor::string_cref_t name ) noexcept : iscene( name, { 10000, 2000 } ) {}
    scene_01_planet( scene_01_planet const & ) = delete;
    scene_01_planet( scene_01_planet && rhv ) noexcept
        : iscene( std::move( rhv ) ), _gltf_mon( motor::move( rhv._gltf_mon ) ),
          _pr( motor::move( rhv._pr ) )
    {
        // @todo ...
    }
    virtual ~scene_01_planet( void_t ) noexcept
    {
        this_t::release_all_objects();
        motor::release( motor::move( _gltf_mon ) );
    }

  public:

    //************************************************************************************
    virtual void_t on_init( demos::iscene::on_init_data_in_t oid ) noexcept
    {
        _db = oid.db;
        _mmgr = oid.mmgr;
        _own_mmgr = motor::shared( motor::gfx::msl_manager_t( motor::share( _db ) ) );
        _gltf_mon = motor::shared( motor::io::monitor_t(), "gltf file monitor" );

        {
            _time = motor::shared( os_float_t( 0.0f ) );
            _scale_os = motor::shared( os_trafo_t() );
        }

        {
            motor::graphics::state_object_t so =
                motor::graphics::state_object_t( "scene.01.final_render_states" );

            {
                motor::graphics::render_state_sets_t rss;
#if 0
                rss.depth_s.do_change = true;
                rss.depth_s.ss.do_activate = true;
                rss.depth_s.ss.do_depth_write = true;
#endif
                rss.polygon_s.do_change = true;
                rss.polygon_s.ss.do_activate = true;
                rss.polygon_s.ss.fm = motor::graphics::fill_mode::fill;
                rss.polygon_s.ss.ff = motor::graphics::front_face::counter_clock_wise;
                rss.polygon_s.ss.cm = motor::graphics::cull_mode::back;
#if 0
                rss.clear_s.do_change = true;
                rss.clear_s.ss.clear_color = motor::math::vec4f_t( 1.0f, 0.0f, 0.0f, 1.0f );
                rss.clear_s.ss.do_activate = true;
                rss.clear_s.ss.do_color_clear = true;
                rss.clear_s.ss.do_depth_clear = true;
#endif
#if 0
                rss.view_s.do_change = true;
                rss.view_s.ss.do_activate = false;
                rss.view_s.ss.vp = motor::math::vec4ui_t( 0, 0, 500, 500 );
#endif
#if 0
                rss.blend_s.do_change = false ;
                rss.blend_s.ss.do_activate = true ;
                rss.blend_s.ss.blend_func = motor::graphics::blend_function::add ;
                rss.blend_s.ss.src_blend_factor = motor::graphics::blend_factor::one ;
                rss.blend_s.ss.dst_blend_factor = motor::graphics::blend_factor::one ;
#endif

                so.add_render_state_set( rss );
            }

            _final_so = motor::shared( motor::graphics::state_object_t( std::move( so ) ) );
        }

        {
            motor::graphics::state_object_t so =
                motor::graphics::state_object_t( "scene.01.root_render_states" );

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

        // light render states
        {
            motor::graphics::state_object_t so =
                motor::graphics::state_object_t( "scene.01.light_render_states" );

            {
                motor::graphics::render_state_sets_t rss;
                rss.depth_s.do_change = true;
                rss.depth_s.ss.do_activate = true;
                rss.depth_s.ss.do_depth_write = false;
                rss.depth_s.ss.compare_funk = motor::graphics::depth_compare::less_equal;

                rss.blend_s.do_change = true;
                rss.blend_s.ss.do_activate = true;
                rss.blend_s.ss.blend_func = motor::graphics::blend_function::add;
                rss.blend_s.ss.dst_blend_factor = motor::graphics::blend_factor::one;
                rss.blend_s.ss.src_blend_factor = motor::graphics::blend_factor::src_alpha;

#if 0
                // this is done in the frame restart of the
                // rendering backends. Here we can not do it
                // because we are using mutiple lights and therefore
                // render the scene scene multiple times.
                rss.clear_s.do_change = true;
                rss.clear_s.ss.clear_color = motor::math::vec4f_t( 0.5f, 0.9f, 0.5f, 1.0f );
                rss.clear_s.ss.do_activate = true;
                rss.clear_s.ss.do_color_clear = true;
                rss.clear_s.ss.do_depth_clear = true;
#endif

#if 0
                // do not need to set the viewport here. It is
                // done through the backends.
                rss.view_s.do_change = true;
                rss.view_s.ss.do_activate = false;
                rss.view_s.ss.vp = motor::math::vec4ui_t( 0, 0, 500, 500 );
#endif

                so.add_render_state_set( rss );
            }

            _light_so = motor::shared( motor::graphics::state_object_t( std::move( so ) ) );
        }

        {
            motor::graphics::state_object_t so =
                motor::graphics::state_object_t( "scene.01.shadow_render_states" );

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

                rss.depth_s.ss.compare_funk = motor::graphics::depth_compare::less;
                rss.clear_s.do_change = true;
                rss.clear_s.ss.clear_color = motor::math::vec4f_t( 0.0f, 0.0f, 0.0f, 1.0f );
                rss.clear_s.ss.do_activate = true;
                rss.clear_s.ss.do_color_clear = true;
                rss.clear_s.ss.do_depth_clear = true;
                rss.view_s.do_change = true;
                rss.view_s.ss.do_activate = true;
                rss.view_s.ss.vp = motor::math::vec4ui_t( 0, 0, 1920 >> 1, 1080 >> 1 );
                so.add_render_state_set( rss );
            }

            _shadow_depth_so = motor::shared( motor::graphics::state_object_t( std::move( so ) ) );
        }

        {
            motor::graphics::state_object_t so =
                motor::graphics::state_object_t( "scene.01.shadow_accum_render_states" );

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

            _shadow_accum_so = motor::shared( motor::graphics::state_object_t( std::move( so ) ) );
        }

        // load scene
        {
            auto const loc = motor::io::location_t( "01_scene_planet.scene.gltf" );
            this_t::init_scene_and_components( loc, oid.mmgr );
            _db->attach( loc, motor::share( _gltf_mon ) );
        }
        {
            _pr = motor::shared( motor::gfx::primitive_render_3d_t() );
            _pr->init( "scene.01.prim_render" );
        }
    }

    //**************************************************************************************
    void_t init_scene_and_components(
        motor::io::location_cref_t loc, motor::gfx::msl_manager_mtr_t mmgr ) noexcept
    {
        // init scene tree
        {
            motor::scene::logic_group_t root;
            root.add_component( motor::shared( motor::scene::name_component_t( "scene.01" ) ) );

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

            auto t = motor::shared( motor::wire::funk_node_t( "end_node",
                [ = ]( motor::wire::funk_node_ptr_t ) { this->_update_sync = 2; } ) );

            _node_dump.emplace_back( motor::share( t ) );
            _merger->then( motor::move( t ) );
        }
        motor::release( motor::move( _selected_node ) );

        // collect msl_components just added and add variable
        {
            demos::msl_component_collector_t v( demos::to_id( demos::msl_id::color_pass_id ) );
            motor::scene::node_t::traverser( _root ).apply( &v );

            v.for_each( [ & ]( motor::scene::msl_component_mtr_t mslc )
            {
                auto & inputs = mslc->borrow_shader_inputs();

                auto is = motor::wire::input_slot< motor::math::vec4f_t >(
                    motor::math::vec4f_t( 1.0f, 0.1f, 0.1f, 1.0f ) );
                inputs.add( "color", motor::shared( std::move( is ) ) );
            } );
        }

        // create sun camera and create shadow framebuffer
        {
            motor::gfx::generic_camera_t gcam;
            _sun_cam = motor::shared( std::move( gcam ) );

            _cameras.emplace_back( std::make_pair( "sun cam", motor::share( _sun_cam ) ) );

            float_t const w = _sun_cam->get_dims().x();
            float_t const h = _sun_cam->get_dims().y();

            auto fb = motor::graphics::framebuffer_object_t( "scene.01.shadow_depth_framebuffer" );
            fb.set_target( motor::graphics::color_target_type::rgba_uint_8, 1 )
                .set_target( motor::graphics::depth_stencil_target_type::depth32 )
                .resize( size_t( 1920 >> 1 ), size_t( 1080 >> 1 ) );

            motor::release( motor::move( _shadow_depth_fb ) );
            _shadow_depth_fb = motor::shared( std::move( fb ) );
        }

        {
            auto fb = motor::graphics::framebuffer_object_t( "scene.01.shadow_accum_framebuffer" );
            fb.set_target( motor::graphics::color_target_type::rgba_uint_8, 1 )
                .set_target( motor::graphics::depth_stencil_target_type::depth32 )
                .resize( size_t( 1000 ), size_t( 1000 ) );

            motor::release( motor::move( _shadow_accum_fb ) );
            _shadow_accum_fb = motor::shared( std::move( fb ) );
        }

        // manager
        {
            _own_mmgr->add( "scene1.color_pass",
                motor::io::location_t( "01_scene_planet.shaders.color_pass.msl" ) );

            _reg0 = mmgr->unregister_funk( _reg0 );
            _reg0 = mmgr->register_funk(
                [ this ]( motor::string_in_t name, motor::graphics::msl_object_mtr_t msl )
            {
                if( name == "light_pass" )
                {
                    motor::scene::add_msl_to_set_visitor_t v(
                        demos::to_id( demos::msl_id::light_pass_id ), motor::share( msl ) );
                    motor::scene::node_t::traverser( _root ).apply( &v );
                }
                else if( name == "shadow_light_pass" )
                {
                    {
                        motor::scene::add_msl_to_set_visitor_t v(
                            demos::to_id( demos::msl_id::shadow_light_pass_id ),
                            motor::share( msl ) );
                        motor::scene::node_t::traverser( _root ).apply( &v );
                    }

                    {
                        motor::scene::connect_msl_slot_visitor_t v(
                            demos::to_id( demos::msl_id::shadow_light_pass_id ),
                            [ & ]( motor::wire::inputs_ref_t inputs )
                        {
#if 1
                            {
                                auto * input = inputs.borrow_or_add(
                                    "bias", motor::shared( motor::wire::is_float_t( 0.02f ) ) );
                                if( input ) input->connect( motor::share( _bias ) );
                            }
                            {
                                auto * input = inputs.borrow_or_add( "light_intensity",
                                    motor::shared( motor::wire::is_float_t( 0.0f ) ) );
                                if( input ) input->connect( motor::share( _light_intensity ) );
                            }

#else
                            {
                                auto * input = inputs.borrow_or_add(
                                    "shininess", motor::shared( this_t::float_is_t( 0.0f ) ) );
                                if( input ) input->connect( motor::share( _shininess ) );
                            }

                            {
                                auto * input = inputs.borrow_or_add( "specular_strength",
                                    motor::shared( this_t::float_is_t( 0.0f ) ) );
                                if( input ) input->connect( motor::share( _specular_strength ) );
                            }

                            {
                                auto * input = inputs.borrow_or_add( "light_intensity",
                                    motor::shared( this_t::float_is_t( 0.0f ) ) );
                                if( input ) input->connect( motor::share( _light_intensity ) );
                            }

                            {
                                auto * input = inputs.borrow_or_add(
                                    "hemi_top_color", motor::shared( this_t::vec3_is_t() ) );
                                if( input ) input->connect( motor::share( _hemi_top_color ) );
                            }
#endif
                        } );
                        motor::scene::node_t::traverser( _root ).apply( &v );
                    }
                }
                else if( name == "shadow_depth_pass" )
                {
                    motor::scene::add_msl_to_set_visitor_t v(
                        demos::to_id( demos::msl_id::shadow_depth_id ), motor::share( msl ) );
                    motor::scene::node_t::traverser( _root ).apply( &v );
                }
                else if( name == "depth_pass" )
                {
                    motor::scene::add_msl_to_set_visitor_t v(
                        demos::to_id( demos::msl_id::depth_pass_id ), motor::share( msl ) );
                    motor::scene::node_t::traverser( _root ).apply( &v );
                }
                else if( name == "normal_vis_pass" )
                {
                    motor::scene::add_msl_to_set_visitor_t v(
                        demos::to_id( demos::msl_id::normal_vis_pass_id ), motor::share( msl ) );
                    motor::scene::node_t::traverser( _root ).apply( &v );
                }
            } );

            _reg1 = _own_mmgr->unregister_funk( _reg1 );
            _reg1 = _own_mmgr->register_funk(
                [ this ]( motor::string_in_t name, motor::graphics::msl_object_mtr_t msl )
            {
                if( name == "scene1.color_pass" )
                {
                    motor::scene::add_msl_to_set_visitor_t v(
                        demos::to_id( demos::msl_id::color_pass_id ), motor::share( msl ) );
                    motor::scene::node_t::traverser( _root ).apply( &v );
                }
            } );
        }

        // attach controllers
        {
            demos::scene_01::rotation_controller_mtr_t ctr =
                motor::shared( demos::scene_01::rotation_controller_t(
                    motor::math::vec3f_t( 1.0f, 1.0f, 1.0f ), 0.1f ) );

            motor::wire::funk_node_mtr_t end = nullptr;

            ctr->borrow_time_is()->connect( motor::share( _time ) );

            motor::scene::for_each_node_visitor_t v(
                [ & ]( motor::scene::for_each_node_visitor_t::for_each_data const & d )
            {
                if( d.name == "outer_ring" )
                {
                    if( auto * comp =
                            d.node->borrow_component< motor::scene::trafo3d_component_t >();
                        comp )
                    {
                        motor::wire::trafo3f_composer_t composer;
                        composer.ensure_and_borrow_rotation_is()->connect(
                            motor::share( ctr->borrow_rotation_os() ) );

                        auto comp_mtr = motor::shared( std::move( composer ) );
                        end = motor::share( comp_mtr );

                        // override. currently only one composer can connect.
                        comp->borrow_trafo_is()->connect( comp_mtr->ensure_and_get_os() );
                        comp->attach_composer( motor::move( comp_mtr ) );
                    }
                    return motor::scene::result::break_traversal;
                }
                return motor::scene::result::ok;
            } );
            motor::scene::node_t::traverser( _root ).apply( &v );

            if( end )
                _time_node->then( ctr->get_node() )
                    ->then( motor::move( end ) )
                    ->then( motor::share( _merger ) );
        }

        {
            _imn_wire.build( _time_node );
        }
    }

    //************************************************************************************
    virtual void_t on_release( demos::iscene::on_release_data_in_t ord ) noexcept
    {

        _reg0 = ord.mmgr->unregister_funk( _reg0 );
        _reg1 = _own_mmgr->unregister_funk( _reg1 );

        this_t::release_all_objects();
    }

    //************************************************************************************
    // start parallel work
    void_t on_update_stage1( demos::iscene::update_data_cref_t ud ) noexcept
    {
        _gltf_mon->for_each_and_swap(
            [ & ]( motor::io::location_cref_t loc, motor::io::monitor_t::notify const n )
        { this_t::init_scene_and_components( loc, ud.mmgr ); } );

        if( _update_sync != 0 ) return;
        _update_sync = 1;

        {
            float_t t = ud.relative_seconds;
            t = motor::math::fn< float_t >::mod( t, 9.0 );
            // t = motor::math::fn<float_t>::abs( t ) ;
            // t = 1.0f - t ;

            _time->set_and_exchange( t );
        }

        {
            motor::concurrent::global_t::schedule(
                _time_node->get_task(), motor::concurrent::schedule_type::pool );
        }
    }

    //************************************************************************************
    // do work after parallel work is done.
    void_t on_update_stage2( demos::iscene::update_data_cref_t ud ) noexcept
    {
        if( _update_sync != 2 ) return;
        _update_sync = 0;
    }

    //************************************************************************************
    virtual void_t on_update( demos::iscene::update_data_cref_t ud ) noexcept
    {
        _own_mmgr->on_update();

        this_t::on_update_stage1( ud );
        this_t::on_update_stage2( ud );
    }

    virtual void_t on_resize(
        demos::window_type const wt, uint_t const width, uint_t const height ) noexcept
    {
    }

    virtual void_t on_graphics( demos::iscene::on_graphics_data_in_t gd ) noexcept
    {

        // sun cam creation
        // here we need the world box which is used to create the othographic box
        // so that the camera can see all the geometry/objects in the scene
        // 1. create loot_at using the light direction
        // 2. transform the world aabb into view space world aabb-> view obb-> view aabb
        // 3. use view aabb for width, height, near, far computation
        // 4. create orthographic projection
        // 5. fix cam position in view space using the view aabb
        // 6. transform new cam position back to world space and assign to camera.
        {
            motor::gfx::directional_light_mtr_t lg = nullptr;

            // find sun light direction
            // in this case, just take the first directional we find and
            // use that as the sun light.
            {
                motor::scene::light_collector_visitor_t vis;
                motor::scene::node_t::traverser( _root ).apply( &vis );
                vis.for_each_light( [ & ]( size_t const i, motor::gfx::light_mtr_t light )
                {
                    if( light->is_shadow_casting() &&
                        light->get_light_type() == motor::gfx::light_type::directional )
                    {
                        lg = static_cast< motor::gfx::directional_light_mtr_t >( light );
                        return false;
                    }
                    return true;
                } );
            }

            if( lg != nullptr )
            {
                // create look at
                auto const dir = lg->get_transformed_direction().normalized();
                _sun_cam->look_at_by_dir( motor::math::vec3f_t(), dir.negated() );

                // transform world aabb to view space => view obb
                auto const view_obb = motor::collide::transform3f_t::from_aabb(
                    _world_bounds, motor::math::m3d::trafof_t( _sun_cam->get_view_matrix() ) );

                // transform view obb to aabb in view space => view aabb
                auto const view_aabb = motor::collide::transform_bounds3f_t::to_aabb( view_obb );

                // now because the box is camera aligned, we can easily grap the values.
                float_t const margin = 0.5f;
                float_t const near_plane = 0.1f;
                float_t const far_plane =
                    near_plane + view_aabb.get_max().z() - view_aabb.get_min().z() + 2.0f * margin;

                // fix projection
                {
                    float_t const width = view_aabb.get_dims().x() + 2.0f * margin;
                    float_t const height = view_aabb.get_dims().y() + 2.0f * margin;

                    _sun_cam->set_dims( width, height, near_plane, far_plane );
                    _sun_cam->orthographic();
                }

                // fix position
                {
                    auto const center = view_aabb.get_center();
                    auto const position_view = motor::math::vec3f_t(
                        center.x(), center.y(), view_aabb.get_max().z() + near_plane + margin );

                    auto const position_world = ( _sun_cam->get_camera_matrix() *
                                                  motor::math::vec4f_t( position_view, 1.0f ) )
                                                    .xyz();

                    _sun_cam->translate_to( position_world );
                }

                // at this point, the sun cam should enclose the whole scene.
            }
        }

        {
            if( _draw_debug )
            {
                motor::scene::debug_graphics_visitor_t::draw_context dc = {
                    _cameras[ _cam_id ].second, _pr, 0, true };

                motor::scene::debug_graphics_visitor_t v( dc );
                motor::scene::node_t::traverser( _root ).apply( &v );

                //_max_depth = v.get_max_depth();
            }

            // render sun_cam frustum
            if( _draw_debug )
            {
                std::array< motor::math::vec3f_t, 8 > points;
                if( _sun_cam->get_frustum().points( points ) )
                {
                    _pr->draw_wire_cube( points, motor::math::vec4f_t( 0.0f, 1.0f, 0.0f, 1.0f ) );
                }
            }
        }

        _pr->set_view_proj( _cameras[ _cam_id ].second->get_view_matrix(),
            _cameras[ _cam_id ].second->get_proj_matrix() );

        _pr->prepare_for_rendering();

        {
            motor::scene::trafo_visitor_t v;
            motor::scene::node_t::traverser( _root ).apply( &v );
            _world_bounds = v.get_world_box();
        }

        {
            motor::scene::variable_update_visitor_t v;
            motor::scene::node_t::traverser( _root ).apply( &v );
        }
    }

    //************************************************************************************
    virtual void_t on_render_init( demos::iscene::on_render_data_in_t rd,
        motor::graphics::gen4::frontend::fence_funk_t funk ) noexcept
    {

        if( rd.wt == demos::window_type::debug )
        {
            rd.fe->configure< motor::graphics::state_object_t >( _root_so );
            _pr->configure( rd.fe );
        }

        if( rd.wt == demos::window_type::production )
        {
            rd.fe->configure< motor::graphics::state_object_t >( _final_so );
            rd.fe->configure< motor::graphics::state_object_t >( _light_so );

            rd.fe->configure< motor::graphics::state_object_t >( _shadow_depth_so );
            rd.fe->configure< motor::graphics::framebuffer_object_t >( _shadow_depth_fb );
            rd.fe->configure< motor::graphics::state_object_t >( _shadow_accum_so );
            rd.fe->configure< motor::graphics::framebuffer_object_t >( _shadow_accum_fb );
        }
        rd.fe->fence( funk );

        // send all msl objects to be configured for this window.
        _own_mmgr->on_render_init( rd.fe );
    }

    //************************************************************************************
    virtual void_t on_render_deinit( demos::iscene::on_render_data_in_t rd,
        motor::graphics::gen4::frontend::fence_funk_t funk ) noexcept
    {
        _own_mmgr->on_render_release( rd.fe );

        if( rd.wt == demos::window_type::debug )
        {
            rd.fe->release< motor::graphics::state_object_t >( _root_so );
            _pr->release( rd.fe );
        }

        if( rd.wt == demos::window_type::production )
        {
            rd.fe->release< motor::graphics::state_object_t >( _light_so );
            rd.fe->release< motor::graphics::state_object_t >( _root_so );
            rd.fe->release< motor::graphics::state_object_t >( _final_so );
            rd.fe->release< motor::graphics::state_object_t >( _shadow_depth_so );
            rd.fe->release< motor::graphics::framebuffer_object_t >( _shadow_depth_fb );
            rd.fe->release< motor::graphics::state_object_t >( _shadow_accum_so );
            rd.fe->release< motor::graphics::framebuffer_object_t >( _shadow_accum_fb );
        }

        rd.fe->fence( funk );
    }

    //************************************************************************************
    virtual void_t on_frame_done( demos::iscene::on_frame_done_data_in_t ) noexcept
    {
        _own_mmgr->on_frame_done();
    }

    //************************************************************************************
    virtual void_t on_render_debug( demos::iscene::on_render_data_in_t rd ) noexcept
    {
        _own_mmgr->on_render( rd.fe );

        if( _cam_id != size_t( -1 ) )
        {
            rd.fe->push( _root_so );
            motor::gfx::generic_camera_mtr_t cam = _cameras[ _cam_id ].second;
            if( _render_normals )
            {
                motor::scene::render_visitor_t vis(
                    demos::to_id( demos::msl_id::normal_vis_pass_id ), rd.fe, cam );
                motor::scene::node_t::traverser( _root ).apply( &vis );
            }
            else
            {

                // cam->set_dims( 1000.0f, 1000.0f, 1.0f, 1000.0f) ;
                motor::scene::render_visitor_t vis(
                    demos::to_id( demos::msl_id::default_id ), rd.fe, cam );
                motor::scene::node_t::traverser( _root ).apply( &vis );
            }
            rd.fe->pop( motor::graphics::gen4::backend::pop_type::render_state );
        }

        _pr->prepare_for_rendering( rd.fe );
        _pr->render( rd.fe );
    }

    //************************************************************************************
    virtual void_t on_render_final_offscreen( demos::iscene::on_render_data_in_t rd ) noexcept
    {
        // make shadow pass for sun light
        if( _sun_cam )
        {
            rd.fe->use( _shadow_depth_fb );
            rd.fe->push( _shadow_depth_so );
            motor::gfx::generic_camera_mtr_t cam = _sun_cam;
            motor::scene::render_visitor_t vis(
                demos::to_id( demos::msl_id::shadow_depth_id ), 0, rd.fe, cam );
            motor::scene::node_t::traverser( _root ).apply( &vis );
            rd.fe->pop( motor::graphics::gen4::backend::pop_type::render_state );
            rd.fe->unuse( motor::graphics::gen4::backend::unuse_type::framebuffer );
        }

// render light source shadow into shadow accum buffer
#if 0
        {
            rd.fe->use( _shadow_accum_fb );
            rd.fe->push( _shadow_accum_so );
            motor::gfx::generic_camera_mtr_t cam = _cameras[ _cam_id ].second;
            motor::scene::render_visitor_t vis(
                demos::to_id( demos::msl_id::shadow_accum_id ), rd.fe, cam );
            motor::scene::node_t::traverser( _root ).apply( &vis );
            rd.fe->pop( motor::graphics::gen4::backend::pop_type::render_state );
            rd.fe->unuse( motor::graphics::gen4::backend::unuse_type::framebuffer );
        }
#endif
    }

    //************************************************************************************
    virtual void_t on_render_final_depth_pass( demos::iscene::on_render_data_in_t rd ) noexcept
    {
        if( _cam_id != size_t( -1 ) )
        {
            rd.fe->push( _final_so );
            {
                motor::gfx::generic_camera_mtr_t cam = _cameras[ _cam_id ].second;
                motor::scene::render_visitor_t vis(
                    demos::to_id( demos::msl_id::depth_pass_id ), rd.fe, cam );

                motor::scene::node_t::traverser( _root ).apply( &vis );
            }

            rd.fe->pop( motor::graphics::gen4::backend::pop_type::render_state );
        }
    }

    //************************************************************************************
    virtual void_t on_render_final( demos::iscene::on_render_data_in_t rd ) noexcept
    {
        _own_mmgr->on_render( rd.fe );

        if( _cam_id != size_t( -1 ) )
        {
            rd.fe->push( _final_so );

#if 1
            size_t render_id = size_t( 0 );

            if( _cam_id != size_t( -1 ) )
            {
                rd.fe->push( _final_so );

#if 0
                {
                    motor::scene::render_visitor_t rnd_vis(
                    demos::to_id( demos::msl_id::default_id ), ++render_id, rd.fe,
                    _cameras[ _cam_id ].second );
                motor::scene::node_t::traverser( _root ).apply( &rnd_vis );
                }
#endif
                {
#if 1
                    rd.fe->push( _light_so );
                    if( _sun_cam )
                    {

                        motor::gfx::directional_light_t dl( _sun_cam->get_direction().negated() );
                        motor::gfx::single_shadow_data_t sd = { _sun_cam->get_proj_matrix(),
                            _sun_cam->get_view_matrix(),
                            "scene.01.shadow_depth_framebuffer.depth" };

                        motor::gfx::generic_camera_mtr_t cam = _cameras[ _cam_id ].second;

                        motor::scene::light_pass_render_visitor_t vis(
                            demos::to_id( demos::msl_id::shadow_light_pass_id ), ++render_id, rd.fe,
                            cam, &dl, &sd );

                        motor::scene::node_t::traverser( _root ).apply( &vis );
                    }
                    rd.fe->pop( motor::graphics::gen4::backend::pop_type::render_state );
#endif
                }

                rd.fe->pop( motor::graphics::gen4::backend::pop_type::render_state );
            }

#else // old
      // if previous render ids were used, set this
      // value appropriately.
            size_t render_id = size_t( 0 );

            // render sun first
            {
                motor::gfx::directional_light_t dl( _sun_cam->get_direction().negated() );
                motor::gfx::single_shadow_data_t sd = { _sun_cam->get_proj_matrix(),
                    _sun_cam->get_view_matrix(), "scene.01.shadow_depth_framebuffer.depth" };

                motor::gfx::generic_camera_mtr_t cam = _cameras[ _cam_id ].second;

                motor::scene::light_pass_render_visitor_t vis(
                    demos::to_id( demos::msl_id::shadow_light_pass_id ), ++render_id, rd.fe, cam,
                    &dl, &sd );

                motor::scene::node_t::traverser( _root ).apply( &vis );
            }

            rd.fe->push( light_so );

            // add all other lights
            {
                motor::scene::light_collector_visitor_t vis;
                motor::scene::node_t::traverser( _root ).apply( &vis );

                motor::gfx::generic_camera_mtr_t cam = _cameras[ _cam_id ].second;

                vis.for_each_light_component(
                    [ & ]( size_t const, motor::scene::light_component_mtr_t light )
                {
                    light->for_each_light( [ & ]( motor::gfx::light_mtr_t l )
                    {
                        motor::scene::light_pass_render_visitor_t vis_light(
                            demos::to_id( demos::msl_id::light_pass_id ), ++render_id, rd.fe, cam,
                            l );
                        motor::scene::node_t::traverser( _root ).apply( &vis_light );
                        return true;
                    } );
                } );
            }

            rd.fe->pop( motor::graphics::gen4::backend::pop_type::render_state );
#endif
            rd.fe->pop( motor::graphics::gen4::backend::pop_type::render_state );
        }
    }

    virtual void_t on_tool( demos::iscene::on_tool_data_in_t ) noexcept
    {

        // SECTION: cameras
        {
            auto cams = _cc.get_cameras();
            cams.emplace_back( std::make_pair( "sun cam", _sun_cam ) );

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

        // render options
        {
            if( ImGui::Begin( "Render Options##scene.01" ) )
            {
                ImGui::Checkbox( "Render Normals#scene.01", &_render_normals );
                ImGui::Checkbox( "Draw Debug#scene.01", &_draw_debug );
            }
            ImGui::End();
        }

        // shadow
        {
            if( ImGui::Begin( "Shadow Options##scene.01" ) )
            {
                {
                    float_t v = _bias->get_value();
                    if( ImGui::SliderFloat( "shadow bias##scene.01", &v, 0.0001f, 0.1f ) )
                    {
                        _bias->set_and_exchange( v );
                    }
                }

                {
                    float_t v = _light_intensity->get_value();
                    if( ImGui::SliderFloat( "light intensity##scene.01", &v, 1.0f, 100.0f ) )
                    {
                        _light_intensity->set_and_exchange( v );
                    }
                }
            }
            ImGui::End();
        }

        // #1 : nodes graph window
        if( ImGui::Begin( "node graph visualization" ) )
        {
            _imn_wire.begin();
            _imn_wire.visualize( 0 );
            _imn_wire.end( true );
        }
        ImGui::End();

#if 0
        ImGui::Separator();

        // SECTION: Scene Graph
        {
            {
                motor::tool::imgui_node_visitor_t v( motor::move( _selected_node ) );
                motor::scene::node_t::traverser( _root ).apply( &v );
                _selected_node = v.move_selected();
            }
        }
#endif
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
        motor::release( motor::move( _light_so ) );

        motor::release( motor::move( _root ) );
        motor::release( motor::move( _selected ) );

        this_t::release_cameras();
        motor::release( motor::move( _selected_node ) );

        this_t::release_node_dump();

        motor::release( motor::move( _sun_cam ) );

        motor::release( motor::move( _shadow_accum_so ) );
        motor::release( motor::move( _shadow_depth_so ) );
        motor::release( motor::move( _shadow_depth_fb ) );
        motor::release( motor::move( _shadow_accum_fb ) );

        motor::release( motor::move( _own_mmgr ) );

        motor::release( motor::move( _pr ) );

        motor::release( motor::move( _bias ) );
        motor::release( motor::move( _light_intensity ) );

        for( auto * ctr : _controllers )
        {
            motor::release( motor::move( ctr ) );
        }
        _controllers.clear();
    }
};
motor_typedef( scene_01_planet );
} // namespace demos
