#pragma once

#include "iscene.h"
#include "../camera_collector.hpp"

#include <motor/scene/node/logic_group.h>
#include <motor/scene/component/name_component.hpp>
#include <motor/scene/component/render_settings_component.h>
#include <motor/scene/component/trafo3d_component.h>
#include <motor/scene/visitor/trafo_visitor.h>
#include <motor/scene/visitor/render_visitor.h>
#include <motor/scene/visitor/variable_update_visitor.h>

#include <motor/graphics/state/render_states.h>

#include <motor/tool/imgui/node_kit/imgui_node_visitor.h>

#include <motor/format/global.h>
#include <motor/log/global.h>
#include <motor/memory/global.h>
#include <motor/concurrent/global.h>

namespace demos
{
class simple_gltf_scene : public iscene
{
    motor_this_typedefs( simple_gltf_scene );

  private:

    motor::io::location_t _asset_location;

    using os_float_t = motor::wire::output_slot< float_t >;
    using os_trafo_t = motor::wire::output_slot< motor::math::m3d::trafof_t >;

    os_float_t * _time = nullptr;
    os_trafo_t * _scale_os = nullptr;

    motor::wire::time_node_mtr_t _time_node = nullptr; // from the importer
    motor::wire::inode_mtr_t _merger = nullptr;        // from the importer

    motor::scene::node_mtr_t _root = nullptr;

    size_t _cam_id = size_t( -1 );
    motor::vector< motor::gfx::generic_camera_mtr_t > _cameras;

    motor::scene::node_mtr_t _selected_node = nullptr;

    demos::camera_collector_t _cc;

    motor::graphics::state_object_mtr_t _root_so = nullptr;

  private: // tool variables

  public:

    simple_gltf_scene( motor::string_cref_t name, motor::io::location_cref_t loc ) noexcept
        : iscene( name ), _asset_location( loc )
    {
    }
    simple_gltf_scene( simple_gltf_scene const & ) = delete;
    simple_gltf_scene( simple_gltf_scene && rhv ) noexcept
        : iscene( std::move( rhv ) ), _asset_location( std::move( rhv._asset_location ) )
    {
    }
    virtual ~simple_gltf_scene( void_t ) noexcept
    {
        this_t::release_all_objects();
    }

  public:

    virtual void_t on_init_cameras( void_t ) noexcept {}
    virtual void_t on_init( motor::io::database_ptr_t db ) noexcept
    {
        {
            _time = motor::shared( os_float_t( 0.0f ) );
            _scale_os = motor::shared( os_trafo_t() );
        }

        {
            motor::graphics::state_object_t so =
                motor::graphics::state_object_t( "root_render_states" );

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

        // #3 : init scene tree
        {
            motor::scene::logic_group_t root;
            root.add_component( motor::shared( motor::scene::name_component_t( "my root name" ) ) );

            // add imported scene
            {
                motor::scene::node_mtr_t imported_node = nullptr;

                auto rs = motor::shared( motor::scene::logic_group_t() );
                {
                    auto rsc =
                        motor::scene::render_settings_component_t( motor::share( _root_so ) );

                    rs->add_component( motor::shared( std::move( rsc ) ) );

                    rs->add_component(
                        motor::shared( motor::scene::name_component_t( "Render Settings" ) ) );
                }

                // make importer ready
                {
                    motor::format::module_registry_mtr_t mod_reg =
                        motor::format::global::register_default_modules(
                            motor::shared( motor::format::module_registry_t(), "mod registry" ) );

                    // import the gltf asset.
                    {
                        motor::property::property_sheet_t ps;
                        ps.add_property< motor::string_t >( "base_name", this_t::name() );

                        auto item = mod_reg->import_from(
                            _asset_location, db, motor::shared( std::move( ps ) ) );
                        auto * ret_item = item.get();

                        // test scene with visitor
                        if( auto * scene_item =
                                dynamic_cast< motor::format::scene_item_ptr_t >( ret_item );
                            scene_item != nullptr )
                        {
                            imported_node = motor::move( scene_item->root );
                            _time_node = motor::move( scene_item->start_node );
                            _merger = motor::move( scene_item->merger_node );

                            _time_node->borrow_time_is()->connect( motor::share( _time ) );
                        }
                        else
                        {
                            motor::log::global_t::critical( "Failed to load gltf file." );
                            std::exit( 1 );
                        }

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

            _root = motor::shared( std::move( root ) );
        }

        {
            motor::scene::node_t::traverser( _root ).apply( &_cc );
            auto cams = _cc.get_cameras();

            if( cams.size() > 0 )
            {
                size_t i = 0;
                _cameras.resize( cams.size() );
                for( auto & c : cams )
                {
                    _cameras[ i++ ] = motor::share( c.second );
                }
                _cam_id = 0;
            }
            else
            {
                motor::log::global_t::error( "gltf file has no cameras." );
            }
        }
    }

    virtual void_t on_release( void_t ) noexcept
    {
        this_t::release_all_objects();
    }

    virtual void_t on_update( demos::iscene::update_data_cref_t ud ) noexcept
    {
        {
            float_t const t = float_t( ud.relative ) / 1000.0f;
            _time->set_and_exchange( t );
        }

        {
            motor::concurrent::global_t::schedule(
                _time_node->get_task(), motor::concurrent::schedule_type::pool );
        }

        // @todo do we have to sync here?

        {
            motor::scene::variable_update_visitor_t v;
            motor::scene::node_t::traverser( _root ).apply( &v );
        }

        {
            motor::scene::trafo_visitor_t v;
            motor::scene::node_t::traverser( _root ).apply( &v );
        }
    }

    virtual void_t on_resize_debug( uint_t const width, uint_t const height ) noexcept {}
    virtual void_t on_resize( uint_t const width, uint_t const height ) noexcept {}

    virtual void_t on_graphics( demos::iscene::on_graphics_data_in_t ) noexcept {}

    virtual void_t on_render_init( demos::window_type const,
        motor::graphics::gen4::frontend_ptr_t fe,
        motor::graphics::gen4::frontend::fence_funk_t funk ) noexcept
    {
        fe->configure< motor::graphics::state_object_t >( _root_so );
        fe->fence( funk );
    }

    virtual void_t on_render_deinit( demos::window_type const,
        motor::graphics::gen4::frontend_ptr_t fe,
        motor::graphics::gen4::frontend::fence_funk_t funk ) noexcept
    {
        fe->fence( funk );
    }

    virtual void_t on_render_debug(
        size_t const wid, motor::graphics::gen4::frontend_ptr_t fe ) noexcept
    {
        if( _cam_id != size_t( -1 ) )
        {
            motor::gfx::generic_camera_mtr_t cam = _cameras[ _cam_id ];
            // cam->set_dims( 1000.0f, 1000.0f, 1.0f, 1000.0f) ;
            motor::scene::render_visitor_t vis( wid, fe, cam );
            motor::scene::node_t::traverser( _root ).apply( &vis );
        }
    }
    virtual void_t on_render_final(
        size_t const wid, motor::graphics::gen4::frontend_ptr_t ) noexcept
    {
    }

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

        ImGui::Separator();

        // SECTION: Scene Graph
        {
            {
                motor::tool::imgui_node_visitor_t v( motor::move( _selected_node ) );
                motor::scene::node_t::traverser( _root ).apply( &v );
                _selected_node = v.move_selected();
            }
        }
    }

  private:

    void_t release_all_objects( void_t ) noexcept
    {
        _cc.release();

        motor::wire::release( motor::move( _time ) );
        motor::wire::release( motor::move( _scale_os ) );

        motor::release( motor::move( _root_so ) );
        motor::release( motor::move( _root ) );
        motor::release( motor::move( _time_node ) );
        motor::release( motor::move( _merger ) );

        for( auto * ptr : _cameras )
        {
            motor::release( motor::move( ptr ) );
        }
        _cameras.clear();
        motor::release( motor::move( _selected_node ) );
    }
};
} // namespace demos