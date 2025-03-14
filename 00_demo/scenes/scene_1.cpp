
#include "scene_1.h"

#include <motor/geometry/mesh/tri_mesh.h>
#include <motor/geometry/mesh/flat_tri_mesh.h>
#include <motor/geometry/3d/cube.h>

#include <motor/math/utility/3d/transformation.hpp>

#include <motor/math/utility/angle.hpp>
#include <motor/concurrent/parallel_for.hpp>

#include <motor/tool/imgui/imgui.h>

using namespace demos ;

//*******************************************************************************
void_t scene_1::on_init_cameras( void_t ) noexcept
{
    ///////////////////////////////////////////////////////////////////////////////
    // Camera section
    ///////////////////////////////////////////////////////////////////////////////

    // camera selector
    {
        using kfs_t = demos::camera_manager::camera_kfs_t ;
        kfs_t kf ;

        kf.insert( kfs_t::keyframe_t( 13000, size_t( 0 ) ) ) ;
        kf.insert( kfs_t::keyframe_t( 20000, size_t( 1 ) ) ) ;
        kf.insert( kfs_t::keyframe_t( 30000, size_t( 1 ) ) ) ;

        this_t::camera_manager().set_camera_selector( std::move( kf ) ) ;
    }

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

            size_t const st = this_t::camera_manager().get_camera_selector()[0].get_time() ;

            {
                using kfs_t = demos::camera_data::keyframe_sequencef_t ;
                kfs_t kf ;
                kf.insert( kfs_t::keyframe_t( st + 0,  motor::math::vec3f_t(103.128769f, 260.683929f, -436.706238f) ) ) ;
                kf.insert( kfs_t::keyframe_t( st + 3000,  motor::math::vec3f_t(2206.015381f, 221.563141f, -569.994934f) ) ) ;
                kf.insert( kfs_t::keyframe_t( st + 9000,  motor::math::vec3f_t(3841.135742f, 168.907684f, 1006.196899f) ) ) ;
                cd.kf_pos = std::move( kf ) ;
            }

            {
                using kfs_t = demos::camera_data::keyframe_sequencef_t ;
                kfs_t kf ;
                kf.insert( kfs_t::keyframe_t( st + 0,  motor::math::vec3f_t(108.288315f, 256.223969f, -417.905518f) ) ) ;
                kf.insert( kfs_t::keyframe_t( st + 3000,  motor::math::vec3f_t(2206.478760f, 218.731476f, -550.202393f) ) ) ;
                kf.insert( kfs_t::keyframe_t( st + 9000,  motor::math::vec3f_t(3839.556885f, 167.649338f, 1026.094116f) ) ) ;

                cd.kf_lookat = std::move( kf ) ;
            }

            this_t::camera_manager().add_camera( std::move( cd ) ) ;
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

            size_t const st = this_t::camera_manager().get_camera_selector()[1].get_time() ;

            {
                auto const start = motor::math::vec3f_t( 1000.0f, 100.0f, 500.0f ) ;

                using kfs_t = demos::camera_data::keyframe_sequencef_t ;
                kfs_t kf_pos ;
                kf_pos.insert( kfs_t::keyframe_t( st, start ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( st + 1000, start + motor::math::vec3f_t( 100.0f, 100.0f, -100.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( st + 2000, start + motor::math::vec3f_t( -400.0f, -500.0f, 1000.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( st + 3000, start + motor::math::vec3f_t( 0.0f, -100.0f, 0.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( st + 4000, start ) ) ;
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

            this_t::camera_manager().add_camera( std::move( cd ) ) ;
        }

        // test : REMOVE IF NOT USED
        #if 0
        {
            this_t::camera_manager().append( { motor::math::vec3f_t( 0 ), motor::math::vec3f_t( 0 ) } ) ;
            this_t::camera_manager().append( { motor::math::vec3f_t( 1 ), motor::math::vec3f_t( 1 ) } ) ;
            this_t::camera_manager().append( { motor::math::vec3f_t( 2 ), motor::math::vec3f_t( 2 ) } ) ;
        }
        #endif
    }
}

//*******************************************************************************
void_t scene_1::on_init( motor::io::database_ptr_t db ) noexcept 
{
    #if 0
    {
        ///////////////////////////////////////////////////////////////////////////////
        // Dummy cube section
        ///////////////////////////////////////////////////////////////////////////////

        // dummy cube
        if( this_t::is_tool_mode() )
        {
            struct vertex { motor::math::vec3f_t pos ; motor::math::vec3f_t nrm ; motor::math::vec2f_t tx ; } ;

            motor::geometry::cube_t::input_params ip ;
            ip.scale = motor::math::vec3f_t( 1.0f ) ;
            ip.tess = 1 ;

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
        if( this_t::is_tool_mode() )
        {
            motor::graphics::msl_object_t mslo( this_t::name() + ".dummy_debug" ) ;

            auto const res = db->load( motor::io::location_t( "shaders.scene_1.dummy_debug.msl" ) ).wait_for_operation(
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
                    trans.set_translation( motor::math::vec3f_t( 0.0f,0.0f,300.0f ) ) ;
                    auto * var = vars.data_variable< motor::math::mat4f_t >( "world" ) ;
                    var->set( trans.get_transformation() ) ;
                }
                mslo.add_variable_set( motor::shared( std::move( vars ), "a variable set 1" ) ) ;
            }

            _dummy_debug_msl = motor::shared( std::move( mslo ) ) ;
        }

        // dummy shader
        if( this_t::is_tool_mode() )
        {
            motor::graphics::msl_object_t mslo( this_t::name() + ".dummy_render" ) ;

            auto const res = db->load( motor::io::location_t( "shaders.scene_1.dummy_final.msl" ) ).wait_for_operation(
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

    }
    #endif

    {
        ///////////////////////////////////////////////////////////////////////////////
        // Render States section
        ///////////////////////////////////////////////////////////////////////////////

        if( this_t::is_tool_mode() )
        {
            motor::graphics::state_object_t so = motor::graphics::state_object_t(
                this_t::name() + ".debug scene" ) ;

            {
                motor::graphics::render_state_sets_t rss ;
                rss.depth_s.do_change = false ;
                rss.depth_s.ss.do_activate = true ;
                rss.depth_s.ss.do_depth_write = true ;

                rss.polygon_s.do_change = false ;
                rss.polygon_s.ss.do_activate = true ;
                rss.polygon_s.ss.ff = motor::graphics::front_face::counter_clock_wise ;
                rss.polygon_s.ss.cm = motor::graphics::cull_mode::back ;

                rss.clear_s.do_change = false ;
                rss.clear_s.ss.clear_color = motor::math::vec4f_t( 0.5f, 0.2f, 0.2f, 1.0f ) ;
                rss.clear_s.ss.do_activate = true ;
                rss.clear_s.ss.do_color_clear = true ;
                rss.clear_s.ss.do_depth_clear = true ;

                so.add_render_state_set( rss ) ;
            }

            _debug_rs = std::move( so ) ;
        }

        // cubes geometry
        {
            struct vertex { motor::math::vec3f_t pos ; motor::math::vec3f_t nrm ; motor::math::vec2f_t tx ; } ;

            motor::geometry::cube_t::input_params ip ;
            ip.scale = motor::math::vec3f_t( 2.0f ) ;
            ip.tess = 0 ;

            motor::geometry::tri_mesh_t tm ;
            motor::geometry::cube_t::make( &tm, ip ) ;

            motor::geometry::flat_tri_mesh_t ftm ;
            tm.flatten( ftm ) ;

            auto vb = motor::graphics::vertex_buffer_t()
                .add_layout_element( motor::graphics::vertex_attribute::position, motor::graphics::type::tfloat, motor::graphics::type_struct::vec3 )
                .add_layout_element( motor::graphics::vertex_attribute::normal, motor::graphics::type::tfloat, motor::graphics::type_struct::vec3 )
                .add_layout_element( motor::graphics::vertex_attribute::texcoord0, motor::graphics::type::tfloat, motor::graphics::type_struct::vec2 )
                .resize( ftm.get_num_vertices() * _max_objects ).update<vertex>( [&] ( vertex * array, size_t const ne )
            {
                motor::concurrent::parallel_for<size_t>( motor::concurrent::range_1d<size_t>( 0, _max_objects ),
                    [&] ( motor::concurrent::range_1d<size_t> const & r )
                {
                    for ( size_t o = r.begin(); o < r.end(); ++o )
                    {
                        size_t const base = o * ftm.get_num_vertices() ;
                        for ( size_t i = 0; i < ftm.get_num_vertices(); ++i )
                        {
                            array[ base + i ].pos = ftm.get_vertex_position_3d( i ) ;
                            array[ base + i ].nrm = ftm.get_vertex_normal_3d( i ) ;
                            array[ base + i ].tx = ftm.get_vertex_texcoord( 0, i ) ;
                        }
                    }
                } ) ;
            } );

            auto ib = motor::graphics::index_buffer_t().
                set_layout_element( motor::graphics::type::tuint ).resize( ftm.indices.size() * _max_objects ).
                update<uint_t>( [&] ( uint_t * array, size_t const ne )
            {
                motor::concurrent::parallel_for<size_t>( motor::concurrent::range_1d<size_t>( 0, _max_objects ),
                    [&] ( motor::concurrent::range_1d<size_t> const & r )
                {
                    for ( size_t o = r.begin(); o < r.end(); ++o )
                    {
                        size_t const vbase = o * ftm.get_num_vertices() ;
                        size_t const ibase = o * ftm.indices.size() ;
                        for ( size_t i = 0; i < ftm.indices.size(); ++i )
                        {
                            array[ ibase + i ] = ftm.indices[ i ] + uint_t( vbase ) ;
                        }
                    }
                } ) ;
            } ) ;

            _cubes_geo = motor::shared( motor::graphics::geometry_object_t( this_t::name() + ".cubes",
                motor::graphics::primitive_type::triangles, std::move( vb ), std::move( ib ) ) ) ;
        }

        // cubes data array
        {
            motor::graphics::data_buffer_t db_ = motor::graphics::data_buffer_t()
                .add_layout_element( motor::graphics::type::tfloat, motor::graphics::type_struct::vec4 )
                .add_layout_element( motor::graphics::type::tfloat, motor::graphics::type_struct::vec4 )
                .add_layout_element( motor::graphics::type::tfloat, motor::graphics::type_struct::vec4 )
                .add_layout_element( motor::graphics::type::tfloat, motor::graphics::type_struct::vec4 )
                .add_layout_element( motor::graphics::type::tfloat, motor::graphics::type_struct::vec4 );

            _cubes_data = motor::shared( motor::graphics::array_object_t( 
                this_t::name() + ".cubes_data", std::move( db_ ) ) ) ;
        }

        // spheres data array
        {
            motor::graphics::data_buffer_t db_ = motor::graphics::data_buffer_t()
                .add_layout_element( motor::graphics::type::tfloat, motor::graphics::type_struct::vec4 ) ;

            _sphere_data = motor::shared( motor::graphics::array_object_t( 
                this_t::name() + ".sphere_data", std::move( db_ ) ) ) ;
        }

        // load common cubes shader libraries
        {
            motor::graphics::msl_object_t mslo;//("scene.1.library") ;
            auto const res = db->load( motor::io::location_t( "shaders.scene_1.cubes.lib.msl" ) ).wait_for_operation(
                    [&] ( char_cptr_t data, size_t const sib, motor::io::result const loading_res )
            {
                if ( loading_res != motor::io::result::ok )
                {
                    assert( false ) ;
                }

                mslo.add( motor::graphics::msl_api_type::msl_4_0, motor::string_t( data, sib ) ) ;
            } ) ;
            _cubes_lib_msl = motor::shared( std::move( mslo ) ) ;
            _reconfigs_debug.emplace_back( _cubes_lib_msl ) ;
            _reconfigs_prod.emplace_back( _cubes_lib_msl ) ;
        }

        // load common sphere shader libraries
        {
            motor::graphics::msl_object_t mslo;//("scene.1.library") ;
            auto const res = db->load( motor::io::location_t( "shaders.scene_1.sphere.lib.msl" ) ).wait_for_operation(
                    [&] ( char_cptr_t data, size_t const sib, motor::io::result const loading_res )
            {
                if ( loading_res != motor::io::result::ok )
                {
                    assert( false ) ;
                }

                mslo.add( motor::graphics::msl_api_type::msl_4_0, motor::string_t( data, sib ) ) ;
            } ) ;
            _sphere_lib_msl = motor::shared( std::move( mslo ) ) ;
            _reconfigs_debug.emplace_back( _sphere_lib_msl ) ;
            _reconfigs_prod.emplace_back( _sphere_lib_msl ) ;
        }

        // msl objects for the cubes
        {
            // cubes debug shader
            if( this_t::is_tool_mode() )
            {
                motor::graphics::msl_object_t mslo( this_t::name() + ".cubes_debug" ) ;

                auto const res = db->load( motor::io::location_t( "shaders.scene_1.cubes_debug.msl" ) ).wait_for_operation(
                    [&] ( char_cptr_t data, size_t const sib, motor::io::result const loading_res )
                {
                    if ( loading_res != motor::io::result::ok )
                    {
                        assert( false ) ;
                    }

                    mslo.add( motor::graphics::msl_api_type::msl_4_0, motor::string_t( data, sib ) ) ;
                } ) ;

                mslo.link_geometry( { this_t::name() + ".cubes" } ) ;
        
                motor::graphics::variable_set_t vars = motor::graphics::variable_set_t() ;
                {
                    auto * var = vars.data_variable< motor::math::vec4f_t >( "color" ) ;
                    var->set( motor::math::vec4f_t( 1.0f, 0.0f, 0.0f, 1.0f ) ) ;
                }

                {
                    auto * var = vars.data_variable< float_t >( "u_time" ) ;
                    var->set( 0.0f ) ;
                }

                {
                    auto * var = vars.texture_variable( "tex" ) ;
                    var->set( "checker_board" ) ;
                }

                {
                    auto * var = vars.array_variable( "u_data" ) ;
                    var->set( this_t::name() + ".cubes_data" ) ;
                }

                mslo.add_variable_set( motor::shared( std::move( vars ), "scene 1: variable set" ) ) ;

                _cubes_debug_msl = motor::shared( std::move( mslo ) ) ;
            }

            // cubes final shader
            {
                motor::graphics::msl_object_t mslo( this_t::name() + ".cubes_final" ) ;

                auto const res = db->load( motor::io::location_t( "shaders.scene_1.cubes_final.msl" ) ).wait_for_operation(
                    [&] ( char_cptr_t data, size_t const sib, motor::io::result const loading_res )
                {
                    if ( loading_res != motor::io::result::ok )
                    {
                        assert( false ) ;
                    }

                    mslo.add( motor::graphics::msl_api_type::msl_4_0, motor::string_t( data, sib ) ) ;
                } ) ;

                mslo.link_geometry( { this_t::name() + ".cubes" } ) ;

                motor::graphics::variable_set_t vars = motor::graphics::variable_set_t() ;
                {
                    auto * var = vars.data_variable< motor::math::vec4f_t >( "color" ) ;
                    var->set( motor::math::vec4f_t( 1.0f, 0.0f, 0.0f, 1.0f ) ) ;
                }

                {
                    auto * var = vars.data_variable< float_t >( "u_time" ) ;
                    var->set( 0.0f ) ;
                }

                {
                    auto * var = vars.texture_variable( "tex" ) ;
                    var->set( "checker_board" ) ;
                }

                {
                    auto * var = vars.array_variable( "u_data" ) ;
                    var->set( this_t::name() + ".cubes_data" ) ;
                }

                mslo.add_variable_set( motor::shared( std::move( vars ) ) ) ;

                _cubes_final_msl = motor::shared( std::move( mslo ) ) ;
            }
        }

        // msl objects for the sphere
        {
            // debug shader
            if( this_t::is_tool_mode() )
            {
                motor::graphics::msl_object_t mslo( this_t::name() + ".sphere_debug" ) ;

                auto const res = db->load( motor::io::location_t( "shaders.scene_1.sphere_debug.msl" ) ).wait_for_operation(
                    [&] ( char_cptr_t data, size_t const sib, motor::io::result const loading_res )
                {
                    if ( loading_res != motor::io::result::ok )
                    {
                        assert( false ) ;
                    }

                    mslo.add( motor::graphics::msl_api_type::msl_4_0, motor::string_t( data, sib ) ) ;
                } ) ;

                mslo.link_geometry( { this_t::name() + ".cubes" } ) ;
        
                motor::graphics::variable_set_t vars = motor::graphics::variable_set_t() ;
                {
                    auto * var = vars.data_variable< motor::math::vec4f_t >( "color" ) ;
                    var->set( motor::math::vec4f_t( 1.0f, 0.0f, 0.0f, 1.0f ) ) ;
                }

                {
                    auto * var = vars.data_variable< float_t >( "u_time" ) ;
                    var->set( 0.0f ) ;
                }

                {
                    auto * var = vars.texture_variable( "tex" ) ;
                    var->set( "checker_board" ) ;
                }

                {
                    auto * var = vars.array_variable( "u_data" ) ;
                    var->set( this_t::name() + ".sphere_data" ) ;
                }

                mslo.add_variable_set( motor::shared( std::move( vars ), "scene 1: variable set" ) ) ;

                _sphere_debug_msl = motor::shared( std::move( mslo ) ) ;
            }

            // cubes final shader
            {
                motor::graphics::msl_object_t mslo( this_t::name() + ".sphere_final" ) ;

                auto const res = db->load( motor::io::location_t( "shaders.scene_1.sphere_final.msl" ) ).wait_for_operation(
                    [&] ( char_cptr_t data, size_t const sib, motor::io::result const loading_res )
                {
                    if ( loading_res != motor::io::result::ok )
                    {
                        assert( false ) ;
                    }

                    mslo.add( motor::graphics::msl_api_type::msl_4_0, motor::string_t( data, sib ) ) ;
                } ) ;

                mslo.link_geometry( { this_t::name() + ".cubes" } ) ;

                motor::graphics::variable_set_t vars = motor::graphics::variable_set_t() ;
                {
                    auto * var = vars.data_variable< motor::math::vec4f_t >( "color" ) ;
                    var->set( motor::math::vec4f_t( 1.0f, 0.0f, 0.0f, 1.0f ) ) ;
                }

                {
                    auto * var = vars.data_variable< float_t >( "u_time" ) ;
                    var->set( 0.0f ) ;
                }

                {
                    auto * var = vars.texture_variable( "tex" ) ;
                    var->set( "checker_board" ) ;
                }

                {
                    auto * var = vars.array_variable( "u_data" ) ;
                    var->set( this_t::name() + ".sphere_data" ) ;
                }

                mslo.add_variable_set( motor::shared( std::move( vars ) ) ) ;

                _sphere_final_msl = motor::shared( std::move( mslo ) ) ;
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Create Random Numbers
        ///////////////////////////////////////////////////////////////////////////////
        {
            for( size_t i=0; i<_random_numbers.size(); ++i ) 
                _random_numbers[i] = float_t(std::rand() % 2000) / 2000.0f ;
        }

        this_t::randomize_lift_points() ;
    }

    {
        _db = motor::share( db ) ;
        _mon = motor::shared( motor::io::monitor_t() ) ;
        _db->attach( motor::share( _mon ) ) ;
    }
}

//*******************************************************************************
void_t scene_1::on_release( void_t ) noexcept
{
    motor::release( motor::move( _cubes_debug_msl ) ) ;
    motor::release( motor::move( _cubes_final_msl ) ) ;
    motor::release( motor::move( _cubes_lib_msl ) ) ;
    motor::release( motor::move( _cubes_geo ) ) ;
    motor::release( motor::move( _cubes_data ) ) ;

    _db->detach( _mon ) ;
    motor::release( motor::move( _mon ) ) ;
    motor::release( motor::move( _db ) ) ;
}

//*******************************************************************************
void_t scene_1::on_resize_debug( uint_t const width, uint_t const height ) noexcept 
{
    _dbg_dims = motor::math::vec2ui_t( width, height ) ;
}

//*******************************************************************************
void_t scene_1::on_resize( uint_t const width, uint_t const height ) noexcept 
{
    _rnd_dims = motor::math::vec2ui_t( width, height ) ;
}

//*******************************************************************************
void_t scene_1::on_update( size_t const cur_time ) noexcept 
{
    this_t::update_time( cur_time ) ;
    // there the camera index is also updated
    this_t::camera_manager().update_camera( cur_time ) ;

    this_t::set_num_objects( _width * _depth ) ;

    _mon->for_each_and_swap( [&]( motor::io::location_cref_t loc, motor::io::monitor_t::notify const n )
    {
        motor::log::global::status<2048>( "[scene.1] : Got %s for %s", motor::io::monitor_t::to_string(n), loc.as_string().c_str() ) ;

        bool_t const a = loc == motor::io::location_t("shaders.scene_1.cubes_debug.msl") ; 
        bool_t const b = loc == motor::io::location_t("shaders.scene_1.cubes.lib.msl") ;
        bool_t const c = loc == motor::io::location_t("shaders.scene_1.sphere.lib.msl") ;
        bool_t const d = loc == motor::io::location_t("shaders.scene_1.sphere_debug.msl") ; 

        motor::string_t shd ;

        if( a || b || c || d )
        {
            _db->load( loc ).wait_for_operation( [&] ( char_cptr_t data, size_t const sib, motor::io::result const ) 
            { 
                shd = motor::string_t( data, sib ) ;
            } ) ;
        }

        if( a )
        {
            _cubes_debug_msl->clear_shaders().add( motor::graphics::msl_api_type::msl_4_0, shd ) ;
            _reconfigs_debug.emplace_back( _cubes_debug_msl ) ;
        }
        else if( b )
        {
            _cubes_lib_msl->clear_shaders().add( motor::graphics::msl_api_type::msl_4_0, shd ) ;
            _reconfigs_debug.emplace_back( _cubes_lib_msl ) ;
            _reconfigs_prod.emplace_back( _cubes_lib_msl ) ;
        }
        else if( c )
        {
            _sphere_lib_msl->clear_shaders().add( motor::graphics::msl_api_type::msl_4_0, shd ) ;
            _reconfigs_debug.emplace_back( _sphere_lib_msl ) ;
            _reconfigs_prod.emplace_back( _sphere_lib_msl ) ;
        }
        else if( d )
        {
            _sphere_debug_msl->clear_shaders().add( motor::graphics::msl_api_type::msl_4_0, shd ) ;
            _reconfigs_debug.emplace_back( _sphere_debug_msl ) ;
        }
    }) ;
}

//*******************************************************************************
void_t scene_1::on_graphics( demos::iscene::on_graphics_data_in_t gd ) noexcept
{
    this_t::coreo_0( gd ) ;

    this_t::camera_manager().for_each_camera( [&] ( size_t const idx, demos::camera_data & cd )
    {
        cd.cam.set_sensor_dims( float_t( _rnd_dims.x() ), float_t( _rnd_dims.y() ) ) ;
        cd.cam.perspective_fov() ;
    } ) ;

    // debug section
    if( this_t::is_tool_mode() )
    {
        auto * cam = this_t::camera_manager().borrow_debug_camera() ;

        auto const view = gd.dbg_cam != nullptr ? gd.dbg_cam->mat_view() : cam->mat_view() ;
        auto const proj = gd.dbg_cam != nullptr ? gd.dbg_cam->mat_proj() : cam->mat_proj() ;

        _cubes_debug_msl->for_each( [&] ( size_t const i, motor::graphics::variable_set_mtr_t vs )
        {
            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "view" ) ;
                var->set( view ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "proj" ) ;
                var->set( proj ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "world" ) ;
                var->set( motor::math::mat4f_t::make_identity() ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::vec2f_t>("u_field") ;
                var->set( motor::math::vec2f_t( float_t( _width ) , float_t( _depth ) ) ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::vec2f_t>("u_wave_amp") ;
                var->set( _wave_amp ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::vec2f_t>("u_wave_freq") ;
                var->set( _wave_freq ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::vec2f_t>("u_wave_phase") ;
                var->set( _wave_phase ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::vec2f_t>("u_poi") ;
                var->set( motor::math::vec2f_t( _poi_x, _poi_z ) ) ;
            }
        } ) ;

        _sphere_debug_msl->for_each( [&] ( size_t const i, motor::graphics::variable_set_mtr_t vs )
        {
            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "view" ) ;
                var->set( view ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "proj" ) ;
                var->set( proj ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "world" ) ;
                var->set( motor::math::mat4f_t::make_identity() ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::vec2f_t>("u_field") ;
                var->set( motor::math::vec2f_t( float_t( _width ) , float_t( _depth ) ) ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::vec2f_t>("u_wave_amp") ;
                var->set( _wave_amp ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::vec2f_t>("u_wave_freq") ;
                var->set( _wave_freq ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::vec2f_t>("u_wave_phase") ;
                var->set( _wave_phase ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::vec2f_t>("u_poi") ;
                var->set( motor::math::vec2f_t( _poi_x, _poi_z ) ) ;
            }
        } ) ;
    }

    if ( this_t::is_tool_mode() )
    {
        auto * cam = this_t::camera_manager().borrow_final_camera() ;

        auto const view = cam->mat_view() ;
        auto const proj = cam->mat_proj() ;
        
    }

    {
        auto * cam = this_t::camera_manager().borrow_final_camera() ;

        auto const view = cam->mat_view() ;
        auto const proj = cam->mat_proj() ;

        _cubes_final_msl->for_each( [&] ( size_t const i, motor::graphics::variable_set_mtr_t vs )
        {
            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "view" ) ;
                var->set( view ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "proj" ) ;
                var->set( proj ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "world" ) ;
                var->set( motor::math::mat4f_t::make_identity() ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::vec2f_t>("u_field") ;
                var->set( motor::math::vec2f_t( float_t( _width ) , float_t( _depth ) ) ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::vec2f_t>("u_wave_amp") ;
                var->set( _wave_amp ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::vec2f_t>("u_wave_freq") ;
                var->set( _wave_freq ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::vec2f_t>("u_wave_phase") ;
                var->set( _wave_phase ) ;
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
void_t scene_1::on_render_init( demos::iscene::render_mode const rm, motor::graphics::gen4::frontend_ptr_t fe ) noexcept 
{
    fe->configure<motor::graphics::array_object>( _cubes_data ) ;
    fe->configure<motor::graphics::geometry_object>( _cubes_geo ) ;
    fe->configure<motor::graphics::array_object>( _sphere_data ) ;

    if ( rm == demos::iscene::render_mode::tool )
    {
        // load library shaders first
        {
            for ( auto * obj : _reconfigs_debug )
            {
                fe->configure<motor::graphics::msl_object_t>( obj ) ;
            }
            _reconfigs_debug.clear() ;
        }

        fe->configure< motor::graphics::state_object_t>( &_debug_rs ) ;
        //fe->configure<motor::graphics::geometry_object>( _dummy_geo ) ;

        fe->configure<motor::graphics::msl_object>( _cubes_debug_msl ) ;
        fe->configure<motor::graphics::msl_object>( _sphere_debug_msl ) ;
    }
    else if ( rm == demos::iscene::render_mode::production )
    {
        // load library shaders first
        {
            for ( auto * obj : _reconfigs_prod )
            {
                fe->configure<motor::graphics::msl_object_t>( obj ) ;
            }
            _reconfigs_prod.clear() ;
        }

        if( this_t::is_tool_mode() )
        {
            //fe->configure< motor::graphics::state_object_t>( &_scene_final_rs ) ;
            //fe->configure<motor::graphics::geometry_object>( _dummy_geo ) ;
        }

        fe->configure<motor::graphics::msl_object>( _cubes_final_msl ) ;
    }
}

//*******************************************************************************
void_t scene_1::on_render_deinit( demos::iscene::render_mode const rm, motor::graphics::gen4::frontend_ptr_t fe ) noexcept 
{
    fe->release( motor::move( _cubes_data ) ) ;
    fe->release( motor::move( _cubes_geo ) ) ;
    

    if ( rm == demos::iscene::render_mode::tool )
    {
        //fe->configure<motor::graphics::msl_object>( _cubes_debug_msl ) ;
        //fe->configure<motor::graphics::msl_object>( _sphere_debug_msl ) ;
        fe->release( motor::move(_cubes_debug_msl) ) ;
        fe->release( motor::move(_sphere_debug_msl) ) ;
    }
    else if ( rm == demos::iscene::render_mode::production )
    {
        // production window but tool mode
        if( this_t::is_tool_mode() )
        {
            
        }

        fe->release( motor::move( _cubes_final_msl ) ) ;
    }
}

//*******************************************************************************
void_t scene_1::on_render_debug( motor::graphics::gen4::frontend_ptr_t fe ) noexcept
{
    {
        for ( auto * obj : _reconfigs_debug )
        {
            fe->configure<motor::graphics::msl_object_t>( obj ) ;
        }
        _reconfigs_debug.clear() ;
    }

    fe->update( _cubes_data ) ;
    fe->update( _sphere_data ) ;

    fe->push( &_debug_rs ) ;
    
    // render cubes
    {
        motor::graphics::gen4::backend_t::render_detail_t detail ;
        detail.start = 0 ;
        detail.num_elems = this_t::num_objects() * 36 ;
        detail.varset = 0 ;
        fe->render( _cubes_debug_msl, detail ) ;
    }
    // render sphere
    {
        motor::graphics::gen4::backend_t::render_detail_t detail ;
        detail.start = 0 ;
        detail.num_elems = this_t::num_sphere_cubes() * 36 ;
        detail.varset = 0 ;
        fe->render( _sphere_debug_msl, detail ) ;
    }
    // render dummy
    {
        motor::graphics::gen4::backend::render_detail_t det ;
        //fe->render( _dummy_debug_msl, det ) ;
    }

    fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
}

//*******************************************************************************
void_t scene_1::on_render_final( motor::graphics::gen4::frontend_ptr_t fe ) noexcept
{
    {
        for ( auto * obj : _reconfigs_prod )
        {
            fe->configure<motor::graphics::msl_object_t>( obj ) ;
        }
        _reconfigs_prod.clear() ;
    }

    fe->update( _cubes_data ) ;
    
    // render scene
    {
        #if 1
        // render cubes
        {
            motor::graphics::gen4::backend_t::render_detail_t detail ;
            detail.start = 0 ;
            detail.num_elems = this_t::num_objects() * 36 ;
            detail.varset = 0 ;
            fe->render( _cubes_final_msl, detail ) ;
        }
        #endif
        //fe->push( &_scene_final_rs ) ;
        {
            motor::graphics::gen4::backend::render_detail_t det ;
            //fe->render( _dummy_render_msl, det ) ;
        }
        //fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
    }
}

//*******************************************************************************
void_t scene_1::on_tool( void_t ) noexcept 
{
    auto slider_int_fn = [&]( char const * name, size_t & io_value, size_t const min, size_t const max )
    {
        int_t v = int_t( io_value ) ;
        if( ImGui::SliderInt( name, &v, int_t(min), int_t( max ) ) )
        {
            io_value = size_t( v ) ;
            return true ;
        }
        return false ;
    } ;

    if( ImGui::Begin( this_t::name().c_str() ) )
    {
        ImGui::Text( "%d:%d", this_t::num_objects(), _max_objects ) ;

        if( slider_int_fn( "Width##scene.1", _width, 10, 400 ) )
        {
            this_t::randomize_lift_points() ;
        }

        if( slider_int_fn( "Depth##scene.1", _depth, 10, 400 ) )
        {
            this_t::randomize_lift_points() ;
        }

        ImGui::SliderFloat( "Cube Scale##scene.1", &_cube_scale, 1.0f, 50.0f ) ;
        ImGui::SliderFloat( "Width Offset##scene.1", &_offset_width, 1.0f, 200.0f ) ;
        ImGui::SliderFloat( "Depth Offset##scene.1", &_offset_depth, 1.0f, 200.0f ) ;

        // select poi
        {
            ImGui::Text( "Point of interest"); ImGui::SameLine() ; ImGui::Separator() ;
            if( ImGui::Checkbox( "Enable##poi.scene.1", &_enable_poi ) ) {}
            ImGui::SliderFloat( "x##poi.scene.1", &_poi_x, 1, float_t(_width) ) ;
            ImGui::SliderFloat( "z##poi.scene.1", &_poi_z, 1, float_t(_depth) ) ;
            ImGui::SliderFloat( "falloff##poi.scene.1", &_falloff_poi, 1, 20 ) ;
        }

        // wave props
        {
            ImGui::Text( "Wave"); ImGui::SameLine() ; ImGui::Separator() ;
            if( ImGui::Checkbox( "Enable##wave.scene.1", &_enable_wave ) ) {}
            ImGui::SliderFloat( "Amplitude##wave.scene.1", &_wave_amp, 1.0f, 200.0f ) ;
            ImGui::SliderFloat( "Frequency##wave.scene.1", &_wave_freq, 1.0f, 50.0f ) ;
            ImGui::SliderFloat( "Phase##wave.scene.1", &_wave_phase, 0.0f, 100.0f ) ;
        }

        // wave props
        {
            ImGui::Text( "Lift Points"); ImGui::SameLine() ; ImGui::Separator() ;
            if( ImGui::Checkbox( "Enable##liftpoints.scene.1", &_enable_lift_points ) ) {}

            ImGui::SliderFloat( "Falloff##liftpoints.scene.1", &_lp_falloff_thres, 1.0f, 10.0f ) ;

        }

        ImGui::Separator() ;
    }
    ImGui::End() ;
}