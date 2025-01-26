
#include "scene_0.h"

#include <motor/geometry/mesh/tri_mesh.h>
#include <motor/geometry/mesh/flat_tri_mesh.h>
#include <motor/geometry/3d/cube.h>

#include <motor/math/utility/3d/transformation.hpp>
#include <motor/math/utility/fn.hpp>
#include <motor/math/utility/angle.hpp>

#include <motor/tool/imgui/imgui.h>

using namespace demos ;

//*******************************************************************************
void_t scene_0::on_init( motor::io::database_ref_t db ) noexcept 
{

    {
        using kfs_t = demos::iscene::camera_kfs_t ;
        kfs_t kf ;

        kf.insert( kfs_t::keyframe_t( 0, size_t( 0 ) ) ) ;
        kf.insert( kfs_t::keyframe_t( 6500, size_t( 1 ) ) ) ;
        kf.insert( kfs_t::keyframe_t( 12000, size_t( 1 ) ) ) ;

        this_t::set_camera_selector( std::move( kf ) ) ;
    }

    // init cameras
    {
        // camera 1
        {
            demos::camera_data cd ;

            cd.cam.set_dims( 1.0f, 1.0f, 1.0f, 10000.0f ) ;
            cd.cam.perspective_fov( motor::math::angle<float_t>::degree_to_radian( 45.0f ) ) ;
            cd.cam.set_sensor_dims( float_t( 1920 ), float_t( 1080 ) ) ;
            cd.cam.look_at( motor::math::vec3f_t( 0.0f, 0.0f, -500.0f ),
                motor::math::vec3f_t( 0.0f, 1.0f, 0.0f ), motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ;

            cd.cam.perspective_fov() ;

            {
                using kfs_t = demos::camera_data::keyframe_sequencef_t ;
                kfs_t kf_pos ;
                kf_pos.insert( kfs_t::keyframe_t( 0, motor::math::vec3f_t( 0.0f, 0.0f, -1000.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( 1000, motor::math::vec3f_t( 1000.0f, 0.0f, -1000.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( 3000, motor::math::vec3f_t( 0.0f, 500.0f, -1000.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( 5000, motor::math::vec3f_t( -1000.0f, -100.0f, -1000.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( 6500, motor::math::vec3f_t( 0.0f, 0.0f, -1000.0f ) ) ) ;
                cd.kf_pos = std::move( kf_pos ) ;
            }

            {
                using kfs_t = demos::camera_data::keyframe_sequencef_t ;

                kfs_t kf( motor::math::time_remap_funk_type::cycle ) ;
                kf.insert( kfs_t::keyframe_t( 0, motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 1000, motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 2000, motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ) ;
                cd.kf_lookat = std::move( kf ) ;
            }

            this_t::add_camera( std::move( cd ) ) ;
        }

        // camera 2
        {
            demos::camera_data cd ;

            cd.cam.set_dims( 1.0f, 1.0f, 1.0f, 10000.0f ) ;
            cd.cam.perspective_fov( motor::math::angle<float_t>::degree_to_radian( 45.0f ) ) ;
            cd.cam.set_sensor_dims( float_t( 1920 ), float_t( 1080 ) ) ;
            cd.cam.look_at( motor::math::vec3f_t( 1000.0f, 100.0f, 500.0f ),
                motor::math::vec3f_t( 0.0f, 1.0f, 0.0f ), motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ;
            //cd.cam.perspective_fov() ;

            {
                auto const start = motor::math::vec3f_t( 1000.0f, 100.0f, 500.0f ) ;
                size_t const start_milli = 6500 ;

                using kfs_t = demos::camera_data::keyframe_sequencef_t ;
                kfs_t kf_pos ;
                kf_pos.insert( kfs_t::keyframe_t( start_milli, start ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( start_milli + 1000, start + motor::math::vec3f_t( 100.0f, 100.0f, -100.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( start_milli + 2000, start + motor::math::vec3f_t( -400.0f, -500.0f, 1000.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( start_milli + 3000, start + motor::math::vec3f_t( 0.0f, -100.0f, 0.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( start_milli + 4000, start ) ) ;
                cd.kf_pos = std::move( kf_pos ) ;
            }

            {
                using kfs_t = demos::camera_data::keyframe_sequencef_t ;

                kfs_t kf( motor::math::time_remap_funk_type::cycle ) ;
                kf.insert( kfs_t::keyframe_t( 0, motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 1000, motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 2000, motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ) ) ;
                cd.kf_lookat = std::move( kf ) ;
            }

            this_t::add_camera( std::move( cd ) ) ;
        }
    }

    // dummy cube
    {
        struct vertex { motor::math::vec3f_t pos ; motor::math::vec3f_t nrm ; motor::math::vec2f_t tx ; } ;

        motor::geometry::cube_t::input_params ip ;
        ip.scale = motor::math::vec3f_t( 1.0f ) ;
        ip.tess = 100 ;

        motor::geometry::tri_mesh_t tm ;
        motor::geometry::cube_t::make( &tm, ip ) ;

        motor::geometry::flat_tri_mesh_t ftm ;
        tm.flatten( ftm ) ;

        auto vb = motor::graphics::vertex_buffer_t()
            .add_layout_element( motor::graphics::vertex_attribute::position, motor::graphics::type::tfloat, motor::graphics::type_struct::vec3 )
            .add_layout_element( motor::graphics::vertex_attribute::normal, motor::graphics::type::tfloat, motor::graphics::type_struct::vec3 )
            .add_layout_element( motor::graphics::vertex_attribute::texcoord0, motor::graphics::type::tfloat, motor::graphics::type_struct::vec2 )
            .resize( ftm.get_num_vertices() ).update<vertex>( [&] ( vertex * array, size_t const ne )
        {
            for ( size_t i = 0; i < ne; ++i )
            {
                array[ i ].pos = ftm.get_vertex_position_3d( i ) ;
                array[ i ].nrm = ftm.get_vertex_normal_3d( i ) ;
                array[ i ].tx = ftm.get_vertex_texcoord( 0, i ) ;
            }
        } );

        auto ib = motor::graphics::index_buffer_t().
            set_layout_element( motor::graphics::type::tuint ).resize( ftm.indices.size() ).
            update<uint_t>( [&] ( uint_t * array, size_t const ne )
        {
            for ( size_t i = 0; i < ne; ++i ) array[ i ] = ftm.indices[ i ] ;
        } ) ;

        _dummy_geo = motor::shared( motor::graphics::geometry_object_t( this_t::name() + ".dummy_object",
            motor::graphics::primitive_type::triangles, std::move( vb ), std::move( ib ) ) ) ;
    }

    // dummy shader debug
    {
        motor::graphics::msl_object_t mslo( this_t::name() + ".dummy_debug" ) ;

        auto const res = db.load( motor::io::location_t( "shaders.dummy_debug.msl" ) ).wait_for_operation(
            [&] ( char_cptr_t data, size_t const sib, motor::io::result const loading_res )
        {
            if ( loading_res != motor::io::result::ok )
            {
                assert( false ) ;
            }

            mslo.add( motor::graphics::msl_api_type::msl_4_0, motor::string_t( data, sib ) ) ;
        } ) ;

        mslo.link_geometry( this_t::name() + ".dummy_object" ) ;

        {
            motor::graphics::variable_set_t vars ;

            {
                auto * var = vars.data_variable< motor::math::vec4f_t >( "color" ) ;
                var->set( motor::math::vec4f_t( 1.0f, 1.0f, 1.0f, 1.0f ) ) ;
            }

            {
                motor::math::m3d::trafof_t trans ;
                trans.set_scale( 100.0f ) ;
                auto * var = vars.data_variable< motor::math::mat4f_t >( "world" ) ;
                var->set( trans.get_transformation() ) ;
            }
            mslo.add_variable_set( motor::shared( std::move( vars ), "a variable set 1" ) ) ;
        }

        _dummy_debug_msl = motor::shared( std::move( mslo ) ) ;
    }

    // dummy shader
    {
        motor::graphics::msl_object_t mslo( this_t::name() + ".dummy_render" ) ;

        auto const res = db.load( motor::io::location_t( "shaders.dummy_final.msl" ) ).wait_for_operation(
            [&] ( char_cptr_t data, size_t const sib, motor::io::result const loading_res )
        {
            if ( loading_res != motor::io::result::ok )
            {
                assert( false ) ;
            }

            mslo.add( motor::graphics::msl_api_type::msl_4_0, motor::string_t( data, sib ) ) ;
        } ) ;

        mslo.link_geometry( this_t::name() + ".dummy_object" ) ;

        {
            motor::graphics::variable_set_t vars ;

            {
                auto * var = vars.data_variable< motor::math::vec4f_t >( "color" ) ;
                var->set( motor::math::vec4f_t( 1.0f, 1.0f, 1.0f, 1.0f ) ) ;
            }

            {
                motor::math::m3d::trafof_t trans ;
                trans.set_scale( 100.0f ) ;
                auto * var = vars.data_variable< motor::math::mat4f_t >( "world" ) ;
                var->set( trans.get_transformation() ) ;
            }
            mslo.add_variable_set( motor::shared( std::move( vars ), "a variable set 1" ) ) ;
        }

        _dummy_render_msl = motor::shared( std::move( mslo ) ) ;
    }

    {
        motor::graphics::state_object_t so = motor::graphics::state_object_t(
            this_t::name() + ".debug scene" ) ;

        {
            motor::graphics::render_state_sets_t rss ;
            rss.depth_s.do_change = true ;
            rss.depth_s.ss.do_activate = true ;
            rss.depth_s.ss.do_depth_write = true ;
            rss.polygon_s.do_change = true ;
            rss.polygon_s.ss.do_activate = true ;
            rss.polygon_s.ss.ff = motor::graphics::front_face::counter_clock_wise ;
            rss.polygon_s.ss.cm = motor::graphics::cull_mode::back ;
            rss.clear_s.do_change = true ;
            rss.clear_s.ss.clear_color = motor::math::vec4f_t( 0.5f, 0.2f, 0.2f, 1.0f ) ;
            rss.clear_s.ss.do_activate = true ;
            rss.clear_s.ss.do_color_clear = true ;
            rss.clear_s.ss.do_depth_clear = true ;

            so.add_render_state_set( rss ) ;
        }

        _debug_rs = std::move( so ) ;
    }
}

//*******************************************************************************
void_t scene_0::on_release( void_t ) noexcept
{
    motor::release( motor::move( _dummy_debug_msl ) ) ;
    motor::release( motor::move( _dummy_render_msl ) ) ;
    motor::release( motor::move( _dummy_geo ) ) ;
}

//*******************************************************************************
void_t scene_0::on_resize_debug( uint_t const width, uint_t const height ) noexcept 
{
    _dbg_dims = motor::math::vec2ui_t( width, height ) ;
}

//*******************************************************************************
void_t scene_0::on_resize( uint_t const width, uint_t const height ) noexcept 
{
    _rnd_dims = motor::math::vec2ui_t( width, height ) ;
}

//*******************************************************************************
void_t scene_0::on_update( size_t const cur_time ) noexcept 
{
    this_t::update_camera( cur_time ) ;
}

//*******************************************************************************
void_t scene_0::on_graphics( demos::iscene::on_graphics_data_in_t gd ) noexcept
{
    this_t::for_each_camera( [&] ( size_t const idx, demos::camera_data & cd )
    {
        cd.cam.set_sensor_dims( float_t( _rnd_dims.x() ), float_t( _rnd_dims.y() ) ) ;
        cd.cam.perspective_fov() ;
    } ) ;

    if( gd.dbg_cam != nullptr ) 
    {
        _dummy_debug_msl->for_each( [&] ( size_t const i, motor::graphics::variable_set_mtr_t vs )
        {
            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "view" ) ;
                var->set( gd.dbg_cam->mat_view() ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "proj" ) ;
                var->set( gd.dbg_cam->mat_proj() ) ;
            }
        } ) ;
    }
    else
    {
        auto * cam = this_t::borrow_debug_camera() ;

        _dummy_debug_msl->for_each( [&] ( size_t const i, motor::graphics::variable_set_mtr_t vs )
        {
            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "view" ) ;
                var->set( cam->mat_view() ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "proj" ) ;
                var->set( cam->mat_proj() ) ;
            }
            #if 0
            {
                auto * var = vs->data_variable<float_t>( "kick" ) ;
                var->set( _aanl.asys.kick ) ;
            }
            #endif
        } ) ;
    }

    // set camera for final render shader
    {
        auto * cam = this_t::borrow_final_camera() ;

        _dummy_render_msl->for_each( [&] ( size_t const i, motor::graphics::variable_set_mtr_t vs )
        {
            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "view" ) ;
                var->set( cam->mat_view() ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "proj" ) ;
                var->set( cam->mat_proj() ) ;
            }
            #if 0
            {
                auto * var = vs->data_variable<float_t>( "kick" ) ;
                var->set( _aanl.asys.kick ) ;
            }
            #endif
        } ) ;
    }
}

//*******************************************************************************
void_t scene_0::on_render_debug( bool_t const initial, motor::graphics::gen4::frontend_ptr_t fe ) noexcept
{
    if( initial )
    {
        fe->configure< motor::graphics::state_object_t>( &_debug_rs ) ;
        fe->configure<motor::graphics::geometry_object>( _dummy_geo ) ;
        fe->configure<motor::graphics::msl_object>( _dummy_debug_msl ) ;
    }

    

    // render
    {
        fe->push( &_debug_rs ) ;
        {
            motor::graphics::gen4::backend::render_detail_t det ;
            fe->render( _dummy_debug_msl, det ) ;
        }
        fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
    }
}

//*******************************************************************************
void_t scene_0::on_render_final( bool_t const initial, motor::graphics::gen4::frontend_ptr_t fe ) noexcept
{
    if( initial )
    {
        //fe->configure< motor::graphics::state_object_t>( &_scene_final_rs ) ;
        fe->configure<motor::graphics::geometry_object>( _dummy_geo ) ;
        fe->configure<motor::graphics::msl_object>( _dummy_render_msl ) ;
    }

    this_t::for_each_camera( [&] ( size_t const idx, demos::camera_data & cd )
    {
        cd.cam.set_sensor_dims( float_t( _rnd_dims.x() ), float_t( _rnd_dims.y() ) ) ;
        cd.cam.perspective_fov() ;
    } ) ;

    // render scene
    {
        //fe->push( &_scene_final_rs ) ;
        {
            motor::graphics::gen4::backend::render_detail_t det ;
            fe->render( _dummy_render_msl, det ) ;
        }
        //fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
    }
}

//*******************************************************************************
void_t scene_0::on_tool( void_t ) noexcept 
{
    if( ImGui::Begin( this_t::name().c_str() ) )
    {
    }
    ImGui::End() ;
}

//*******************************************************************************
void_t scene_0::on_shutdown( void_t ) noexcept 
{

}