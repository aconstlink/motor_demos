
#include "scene_0.h"

#include <motor/geometry/mesh/tri_mesh.h>
#include <motor/geometry/mesh/flat_tri_mesh.h>
#include <motor/geometry/3d/cube.h>

#include <motor/math/utility/3d/transformation.hpp>

#include <motor/math/utility/angle.hpp>
#include <motor/concurrent/parallel_for.hpp>
#include <motor/concurrent/task/task.hpp>

#include <motor/tool/imgui/imgui.h>


using namespace demos ;

//*******************************************************************************
void_t scene_0::on_init_cameras( void_t ) noexcept 
{
    ///////////////////////////////////////////////////////////////////////////////
    // Camera section
    ///////////////////////////////////////////////////////////////////////////////

    // camera selector
    {
        using kfs_t = demos::camera_manager::camera_kfs_t ; 
        kfs_t kf ;

        kf.insert( kfs_t::keyframe_t( 0, size_t( 0 ) ) ) ;
        kf.insert( kfs_t::keyframe_t( 8000, size_t( 1 ) ) ) ;
        kf.insert( kfs_t::keyframe_t( 15000, size_t( 1 ) ) ) ;

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

            {
                using kfs_t = demos::camera_data::keyframe_sequencef_t ;
                kfs_t kf ;
                kf.insert( kfs_t::keyframe_t( 0, motor::math::vec3f_t( 1050.910278f, -0.800429f, -182.960831f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 1000, motor::math::vec3f_t( 534.924927f, -64.362190f, -130.895432f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 3000, motor::math::vec3f_t( 328.392639f, 198.827835f, -395.404388f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 5000, motor::math::vec3f_t( -205.188736f, -52.714512f, -109.579094f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 6500, motor::math::vec3f_t( -908.151306f, 206.600845f, 28.690346f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 8000, motor::math::vec3f_t( -1576.810669f, -12.273022f, 308.882080f ) ) ) ;
                cd.kf_pos = std::move( kf ) ;
            }

            {
                using kfs_t = demos::camera_data::keyframe_sequencef_t ;
                kfs_t kf ;
                kf.insert( kfs_t::keyframe_t( 0, motor::math::vec3f_t( 1031.214111f, -0.796436f, -179.487778f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 1000, motor::math::vec3f_t( 515.552063f, -59.720646f, -132.669312f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 3000, motor::math::vec3f_t( 312.900757f, 190.665634f, -385.741302f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 5000, motor::math::vec3f_t( -224.431519f, -47.709923f, -107.419807f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 6500, motor::math::vec3f_t( -927.006409f, 203.097549f, 34.364937f ) ) ) ;
                kf.insert( kfs_t::keyframe_t( 8000, motor::math::vec3f_t( -1594.317627f, -2.607837f, 308.598633f ) ) ) ;

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

            {
                auto const start = motor::math::vec3f_t( 1000.0f, 100.0f, 500.0f ) ;
                size_t const start_milli = 8000 ;

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
void_t scene_0::on_init( motor::io::database_mtr_t db ) noexcept 
{
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
            motor::graphics::primitive_type::triangles, std::move( vb ), std::move( ib ) ), "dummy geo" ) ;
    }

    // dummy shader debug
    if( this_t::is_tool_mode() )
    {
        motor::graphics::msl_object_t mslo( this_t::name() + ".dummy_debug" ) ;

        auto const res = db->load( motor::io::location_t( "shaders.dummy_debug.msl" ) ).wait_for_operation(
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

        _dummy_debug_msl = motor::shared( std::move( mslo ), "msl dummy" ) ;
    }

    // dummy shader
    if( this_t::is_tool_mode() )
    {
        motor::graphics::msl_object_t mslo( this_t::name() + ".dummy_render" ) ;

        auto const res = db->load( motor::io::location_t( "shaders.dummy_final.msl" ) ).wait_for_operation(
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

        _dummy_render_msl = motor::shared( std::move( mslo ), "dummy_render_msl" ) ;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Render States section
    ///////////////////////////////////////////////////////////////////////////////

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

    ///////////////////////////////////////////////////////////////////////////////
    // Cubes section : Worm
    ///////////////////////////////////////////////////////////////////////////////
    {
        auto const start = motor::math::vec3f_t( 0.0f, 0.0f, 0.0f ) ;
        size_t const start_milli = 0 ;

        using kfs_t = demos::camera_data::keyframe_sequencef_t ;
        kfs_t kf_pos ;
        kf_pos.insert( kfs_t::keyframe_t( start_milli, start ) ) ;
        kf_pos.insert( kfs_t::keyframe_t( start_milli + 1000, start + motor::math::vec3f_t( -1000.0f, 0.0f, 500.0f ) ) ) ;
        kf_pos.insert( kfs_t::keyframe_t( start_milli + 2000, start + motor::math::vec3f_t( -2000.0f, 0.0f, 0.0f ) ) ) ;
        kf_pos.insert( kfs_t::keyframe_t( start_milli + 3000, start + motor::math::vec3f_t( -2000.0f, 0.0f, 1000.0f ) ) ) ;
        kf_pos.insert( kfs_t::keyframe_t( start_milli + 4000, start + motor::math::vec3f_t( 2000.0f, 0.0f, 1000.0f ) ) ) ;
        kf_pos.insert( kfs_t::keyframe_t( start_milli + 5000, start + motor::math::vec3f_t( 2000.0f, 0.0f, 0.0f ) ) ) ;
        kf_pos.insert( kfs_t::keyframe_t( start_milli + 6000, start ) ) ;
        _worm_pos = std::move( kf_pos ) ;
    }

    {
        auto sp = this_t::vec3splinef_t() ;
        sp.append( motor::math::vec3f_t( -2000.0f, 0.0f, 300.0f ) ) ;
        sp.append( motor::math::vec3f_t( -1000.0f, 200.0f, 100.0f ) ) ;
        sp.append( motor::math::vec3f_t( -500.0f, -50.0f, -100.0f ) ) ;
        sp.append( motor::math::vec3f_t( 100.0f, 50.0f, -300.0f ) ) ;
        sp.append( motor::math::vec3f_t( 500.0f, -100.0f, -100.0f ) ) ;
        sp.append( motor::math::vec3f_t( 1000.0f, 0.0f, -100.0f ) ) ;

        _worm_pos_spline = std::move( sp ) ;
    }

    // cubes geometry
    {
        struct vertex { motor::math::vec3f_t pos ; motor::math::vec3f_t nrm ; motor::math::vec2f_t tx ; } ;

        motor::geometry::cube_t::input_params ip ;
        ip.scale = motor::math::vec3f_t( 1.0f ) ;
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

        _cubes_geo = motor::graphics::geometry_object_t( this_t::name() + ".cubes",
            motor::graphics::primitive_type::triangles, std::move( vb ), std::move( ib ) ) ;
    }

    // cubes data array
    {
        motor::graphics::data_buffer_t db_ = motor::graphics::data_buffer_t()
            .add_layout_element( motor::graphics::type::tfloat, motor::graphics::type_struct::vec4 )
            .add_layout_element( motor::graphics::type::tfloat, motor::graphics::type_struct::vec4 )
            .add_layout_element( motor::graphics::type::tfloat, motor::graphics::type_struct::vec4 )
            .add_layout_element( motor::graphics::type::tfloat, motor::graphics::type_struct::vec4 )
            .add_layout_element( motor::graphics::type::tfloat, motor::graphics::type_struct::vec4 );

        _cubes_data = motor::graphics::array_object_t( this_t::name() + ".cubes_data", std::move( db_ ) ) ;
    }

    // msl objects
    {
        // cubes debug shader
        if( this_t::is_tool_mode() )
        {
            motor::graphics::msl_object_t mslo( this_t::name() + ".cubes_debug" ) ;

            auto const res = db->load( motor::io::location_t( "shaders.scene_0.cubes_debug.msl" ) ).wait_for_operation(
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

            _cubes_debug_msl = motor::shared( std::move( mslo ), "cubes msl debug" ) ;
        }

        // cubes final shader
        {
            motor::graphics::msl_object_t mslo( this_t::name() + ".cubes_final" ) ;

            auto const res = db->load( motor::io::location_t( "shaders.scene_0.cubes_final.msl" ) ).wait_for_operation(
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

            mslo.add_variable_set( motor::shared( std::move( vars ), "varset" ) ) ;

            _cubes_final_msl = motor::shared( std::move( mslo ), "cubes final" ) ;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Position section : position functions
    ///////////////////////////////////////////////////////////////////////////////
    {
        _pos_funks.emplace_back( [&]( float_t const t )
        {
            return _worm_pos_spline(t) ;
        } ) ;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Create Random Numbers
    ///////////////////////////////////////////////////////////////////////////////
    {
        for( size_t i=0; i<_random_numbers.size(); ++i ) 
            _random_numbers[i] = float_t(std::rand() % 2000) / 2000.0f ;
    }

    this_t::set_num_objects(100) ;
}

//*******************************************************************************
void_t scene_0::on_release( void_t ) noexcept
{
    motor::release( motor::move( _dummy_render_msl ) ) ;
    motor::release( motor::move( _dummy_debug_msl ) ) ;
    motor::release( motor::move( _dummy_geo ) ) ;
    motor::release( motor::move( _cubes_debug_msl ) ) ;
    motor::release( motor::move( _cubes_final_msl ) ) ;
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
    this_t::update_time( cur_time ) ;
    // there the camera index is also updated
    this_t::camera_manager().update_camera( cur_time ) ;
}

//*******************************************************************************
void_t scene_0::on_graphics( demos::iscene::on_graphics_data_in_t gd ) noexcept
{
    this_t::coreo_1( gd ) ;

    // draw worm path
    #if 1
    if( this_t::is_tool_mode() )
    {
        auto const spline = _worm_pos_spline;

        // path
        {
            size_t const num_steps = 300 ;
            gd.pr->draw_lines( num_steps, [&] ( size_t const i )
            {
                float_t const t0 = float_t( i + 0 ) / float_t ( num_steps - 1 ) ;
                float_t const t1 = float_t( i + 1 ) / float_t ( num_steps - 1 ) ;

                auto const v0 = spline( t0 ) ;
                auto const v1 = spline( t1 ) ;

                return motor::gfx::line_render_3d::draw_line_data { v0, v1, motor::math::vec4f_t( 1.0f ) } ;
            } ) ;
        }

        // frame
        {
            size_t const num_steps = 80 ;
            gd.pr->draw_lines( num_steps, [&] ( size_t const idx )
            {
                size_t const div = 3 ;
                size_t const i = idx / div ;

                float_t const t0 = float_t( i + 0 ) / float_t ( (num_steps/div) - 1 ) ;
                float_t const t1 = float_t( i + 1 ) / float_t ( (num_steps/div) - 1 ) ;

                auto const v0 = spline( t0 ) ;
                auto const v1 = spline( t1 ) ;

                auto const up = motor::math::vec3f_t(0.0f, 1.0f, 0.0f) ;
                auto const dir = ( v1 - v0 ).normalized() ;
                auto const ortho = (up - (dir * dir.dot( up ))).normalized() ;
                auto const right = dir.crossed( ortho ).normalized() ;

                size_t const selector = idx % div ;

                motor::math::vec4f_t const colors[ div ] = 
                {
                    motor::math::vec4f_t( 1.0f, 0.0f, 0.0f, 1.0f ),
                    motor::math::vec4f_t( 0.0f, 1.0f, 0.0f, 1.0f ),
                    motor::math::vec4f_t( 0.0f, 0.0f, 1.0f, 1.0f )
                } ;
                
                motor::math::vec3f_t const norms[ div ] = 
                {
                    dir, ortho, right
                } ;

                auto off = v0 + norms[selector] * 10.0f ;

                return motor::gfx::line_render_3d::draw_line_data
                { v0, off, colors[selector] } ;
                
            } ) ;
        }
    }
    #else
    {
        using splinef_t = this_t::vec3splinef_t ;
        splinef_t spline = _worm_pos.get_spline() ;

        size_t const num_steps = 300 ;
        gd.pr->draw_lines( num_steps, [&] ( size_t const i )
        {
            float_t const t0 = float_t( i + 0 ) / float_t ( num_steps - 1 ) ;
            float_t const t1 = float_t( i + 1 ) / float_t ( num_steps - 1 ) ;

            auto const v0 = spline( t0 ) ;
            auto const v1 = spline( t1 ) ;

            return motor::gfx::line_render_3d::draw_line_data { v0, v1, motor::math::vec4f_t(1.0f) } ;
        } ) ;
    }
    #endif

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

        _dummy_debug_msl->for_each( [&] ( size_t const i, motor::graphics::variable_set_mtr_t vs )
        {
            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "view" ) ;
                var->set( view ) ;
            }

            {
                auto * var = vs->data_variable<motor::math::mat4f_t>( "proj" ) ;
                var->set( proj ) ;
            }
            #if 0
            {
                auto * var = vs->data_variable<float_t>( "kick" ) ;
                var->set( _aanl.asys.kick ) ;
            }
            #endif
        } ) ;

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
        } ) ;
    }

    // set camera for final render shader
    {
        auto * cam = this_t::camera_manager().borrow_final_camera() ;

        auto const view = cam->mat_view() ;
        auto const proj = cam->mat_proj() ;

        if( this_t::is_tool_mode() )
        {
            _dummy_render_msl->for_each( [&] ( size_t const i, motor::graphics::variable_set_mtr_t vs )
            {
                {
                    auto * var = vs->data_variable<motor::math::mat4f_t>( "view" ) ;
                    var->set( view ) ;

                }
                {
                    auto * var = vs->data_variable<motor::math::mat4f_t>( "proj" ) ;
                    var->set( proj ) ;
                }
                #if 0
                {
                    auto * var = vs->data_variable<float_t>( "kick" ) ;
                    var->set( _aanl.asys.kick ) ;
                }
                #endif
            } ) ;
        }

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

            #if 0
            {
                auto * var = vs->data_variable<float_t>( "kick" ) ;
                var->set( _aanl.asys.kick ) ;
            }
            #endif
        } ) ;
    }
    this_t::set_num_objects( _num_rings * _cubes_per_ring ) ;
}

//*******************************************************************************
void_t scene_0::on_render_init( demos::iscene::render_mode const rm, motor::graphics::gen4::frontend_ptr_t fe ) noexcept 
{
    if( rm == demos::iscene::render_mode::tool )
    {
        fe->configure< motor::graphics::state_object_t>( &_debug_rs ) ;
        fe->configure<motor::graphics::geometry_object>( _dummy_geo ) ;
        fe->configure<motor::graphics::msl_object>( _dummy_debug_msl ) ;

        fe->configure<motor::graphics::array_object>( &_cubes_data ) ;
        fe->configure<motor::graphics::geometry_object>( &_cubes_geo ) ;
        fe->configure<motor::graphics::msl_object>( _cubes_debug_msl ) ;
    }
    else if ( rm == demos::iscene::render_mode::production )
    {
        //fe->configure< motor::graphics::state_object_t>( &_scene_final_rs ) ;
        //fe->configure<motor::graphics::geometry_object>( _dummy_geo ) ;
        //fe->configure<motor::graphics::msl_object>( _dummy_render_msl ) ;

        fe->configure<motor::graphics::array_object>( &_cubes_data ) ;
        fe->configure<motor::graphics::geometry_object>( &_cubes_geo ) ;
        fe->configure<motor::graphics::msl_object>( _cubes_final_msl ) ;
    }
}

//*******************************************************************************
void_t scene_0::on_render_deinit( demos::iscene::render_mode const rm, motor::graphics::gen4::frontend_ptr_t fe ) noexcept 
{
    if ( rm == demos::iscene::render_mode::tool )
    {
        fe->release( motor::move( _cubes_debug_msl ) ) ;
        fe->release( motor::move( _dummy_debug_msl ) ) ;
        fe->release( motor::move( _dummy_geo  ) ) ;
        fe->fence( [&]( void_t )
        {
            //motor::release( motor::move( _dummy_debug_msl )  ) ;
            //motor::release( motor::move( _dummy_geo ) ) ;
        } ) ;
        
        //_dummy_debug_msl = nullptr ;
    }
    else if ( rm == demos::iscene::render_mode::production )
    {
        //motor::release( motor::move( _dummy_render_msl ) ) ;
        
        if( this_t::is_production_mode() )
        {
            //fe->release( motor::move( _dummy_geo ) ) ;
        }

        fe->fence( [=] ( void_t )
        {
            
            
        } ) ;
        _dummy_debug_msl = nullptr ;
    }
}

//*******************************************************************************
void_t scene_0::on_render_debug( motor::graphics::gen4::frontend_ptr_t fe ) noexcept
{
    // render
    {
        fe->update( &_cubes_data ) ;

        fe->push( &_debug_rs ) ;
        
        // render cubes
        {
            motor::graphics::gen4::backend_t::render_detail_t detail ;
            detail.start = 0 ;
            detail.num_elems = this_t::num_objects() * 36 ;
            detail.varset = 0 ;
            fe->render( _cubes_debug_msl, detail ) ;
        }

        // render dummy
        {
            motor::graphics::gen4::backend::render_detail_t det ;
            fe->render( _dummy_debug_msl, det ) ;
        }
        fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
    }
}

//*******************************************************************************
void_t scene_0::on_render_final( motor::graphics::gen4::frontend_ptr_t fe ) noexcept
{
    fe->update( &_cubes_data ) ;

    // render scene
    {
        // render cubes
        {
            motor::graphics::gen4::backend_t::render_detail_t detail ;
            detail.start = 0 ;
            detail.num_elems = this_t::num_objects() * 36 ;
            detail.varset = 0 ;
            fe->render( _cubes_final_msl, detail ) ;
        }

        #if 0
        //fe->push( &_scene_final_rs ) ;
        {
            motor::graphics::gen4::backend::render_detail_t det ;
            fe->render( _dummy_render_msl, det ) ;
        }
        //fe->pop( motor::graphics::gen4::backend::pop_type::render_state ) ;
        #endif
    }
}

//*******************************************************************************
void_t scene_0::on_tool( void_t ) noexcept 
{
    auto slider_int_fn = [&]( char const * name, size_t & io_value, size_t const min, size_t const max )
    {
        int_t v = int_t( io_value ) ;
        ImGui::SliderInt( name, &v, int_t(min), int_t( max ) ) ;
        io_value = size_t( v ) ;
    } ;

    if( ImGui::Begin( this_t::name().c_str() ) )
    {
        ImGui::Text("Worm") ;
        {
            ImGui::Text( "%d:%d", this_t::num_objects(), _max_objects ) ; ImGui::SameLine() ;
            ImGui::ColorButton( "##numobjectsok.scene0", this_t::num_objects() == _max_objects ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f ) : 
            ImVec4(0.0f, 1.0f, 0.0f, 1.0f ) ) ;
            // general props
            {
                ImGui::Text("General Properties") ;
                slider_int_fn( "Number of Rings##Scene.0",_num_rings, 0, 300 ) ;
                slider_int_fn( "Cubes per ring##Scene.0",_cubes_per_ring, 10, 200 ) ;
            }

            ImGui::Separator() ;
            // pos funk
            {
                int_t const max_funks = int_t( _pos_funks.size() - 1 ) ;
                int_t cur_funk = (int_t) _cur_pos_funk ;
                ImGui::SliderInt( "Pos funk##Scene.0", &cur_funk, 0, max_funks ) ;
                _cur_pos_funk = size_t( cur_funk ) ;
            }

            {
                ImGui::SliderFloat( "Cube Radius##scene0", &_cube_radius, 10.0f, 50.0f ) ;
            }

            {
                
            }

            {
                slider_int_fn( "Per Ring Time##scene0", _per_ring_milli, 10, 1000 ) ;
            }

            ImGui::Separator() ;
            {
                ImGui::Text("Directional Shifting") ;
                ImGui::SliderFloat( "Inner Amplitude##scene0", &_inner_amp, 1.0f, 100.0f ) ;
                ImGui::SliderFloat( "Inner Frequency##scene0", &_inner_freq, 1.0f, 50.0f ) ;
                ImGui::SliderFloat( "Inner Shift##scene0", &_inner_shift, 0.0f, 10.0f ) ;
                ImGui::SliderFloat( "Direction Rand Shift Multip##scene0", &_direction_shift_rand_mult, 0.0f, 50.0f ) ;
                ImGui::SliderFloat( "Directional Offset##scene0", &_direction_offset, 0.0f, 1.0f ) ; ImGui::SameLine() ;
                ImGui::Checkbox( "##clampscene0directionaloffset", &_clamp_directionl_offset ) ;
            }
            ImGui::Separator() ;

            {
                ImGui::Text("Ring Lift up") ;
                slider_int_fn( "Rotation: Which Ring##scene0", _ring_rotate, size_t(-1), this_t::num_rings() ) ;
                slider_int_fn( "Lift up: Which Ring##scene0", _ring_to_lift, size_t(-1), this_t::num_rings() ) ;
                slider_int_fn( "Lift up Range: Which Ring##scene0", _ring_to_lift_range, size_t(0), 10 ) ;
                ImGui::SliderFloat( "Lift Radius##scene0", &_ring_lift_radius, 0.0f, 50.0f ) ;
                
            }

            ImGui::Separator() ;
            {
                ImGui::Text("Center Radius Properties") ;
                ImGui::SliderFloat( "Max Center Radius##scene0", &_max_center_radius, 10.0f, 1000.0f ) ;
                ImGui::SliderFloat( "Center Radius Multiplier##scene0", &_center_radius, 20.0f, 500.0f ) ;
                ImGui::SliderFloat( "Center Radius Randomizer##scene0", &_center_rand_radius, 0.0f, 100.0f ) ;
                ImGui::SliderFloat( "Center Radius Randomizer Effect##scene0", &_center_rand_radius_mult, 0.0f, 1.0f ) ;
            }
        }
        ImGui::Separator() ;
    }
    ImGui::End() ;
}