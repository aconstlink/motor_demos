
#include "scene_0.h"

#include <motor/geometry/mesh/tri_mesh.h>
#include <motor/geometry/mesh/flat_tri_mesh.h>
#include <motor/geometry/3d/cube.h>

#include <motor/math/utility/3d/transformation.hpp>
#include <motor/math/utility/fn.hpp>
#include <motor/math/utility/angle.hpp>

#include <motor/tool/imgui/imgui.h>
#include <motor/concurrent/parallel_for.hpp>

using namespace demos ;

//*******************************************************************************
void_t scene_0::on_init( motor::io::database_ref_t db ) noexcept 
{
    ///////////////////////////////////////////////////////////////////////////////
    // Camera section
    ///////////////////////////////////////////////////////////////////////////////

    // camera selector
    {
        using kfs_t = demos::camera_manager::camera_kfs_t ;
        kfs_t kf ;

        kf.insert( kfs_t::keyframe_t( 0, size_t( 0 ) ) ) ;
        kf.insert( kfs_t::keyframe_t( 6500, size_t( 1 ) ) ) ;
        kf.insert( kfs_t::keyframe_t( 12000, size_t( 1 ) ) ) ;

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
                kfs_t kf_pos ;
                kf_pos.insert( kfs_t::keyframe_t( 0, motor::math::vec3f_t( 0.0f, 0.0f, -1000.0f ) ) ) ;
                kf_pos.insert( kfs_t::keyframe_t( 1000, motor::math::vec3f_t( 1110.0f, 0.0f, -1000.0f ) ) ) ;
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

            this_t::camera_manager().add_camera( std::move( cd ) ) ;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Dummy cube section
    ///////////////////////////////////////////////////////////////////////////////

    // dummy cube
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

    ///////////////////////////////////////////////////////////////////////////////
    // Render States section
    ///////////////////////////////////////////////////////////////////////////////

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
        sp.append( motor::math::vec3f_t( -1000.0f, 0.0f, 100.0f ) ) ;
        sp.append( motor::math::vec3f_t( -500.0f, 100.0f, 100.0f ) ) ;
        sp.append( motor::math::vec3f_t( -0.0f, 0.0f, 0.0f ) ) ;
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
        struct the_data
        {
            motor::math::vec4f_t pos ;
            motor::math::vec4f_t col ;
            motor::math::vec4f_t d ;
            motor::math::vec4f_t e;
            motor::math::vec4f_t f ;
        };

        float_t scale = 20.0f ;
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
        {
            motor::graphics::msl_object_t mslo( this_t::name() + ".cubes_debug" ) ;

            auto const res = db.load( motor::io::location_t( "shaders.scene_0.cubes_debug.msl" ) ).wait_for_operation(
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

            _cubes_debug_msl = std::move( mslo ) ;
        }

        // cubes final shader
        {
            motor::graphics::msl_object_t mslo( this_t::name() + ".cubes_final" ) ;

            auto const res = db.load( motor::io::location_t( "shaders.scene_0.cubes_final.msl" ) ).wait_for_operation(
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

            _cubes_final_msl = std::move( mslo ) ;
        }
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
    // there the camera index is also updated
    this_t::camera_manager().update_camera( cur_time ) ;
}

//*******************************************************************************
void_t scene_0::on_graphics( demos::iscene::on_graphics_data_in_t gd ) noexcept
{
    // update array object data
    {
        static float_t  angle_ = 0.0f ;
        angle_ += ( ( ( ( gd.dt ) ) ) * 2.0f * motor::math::constants<float_t>::pi() ) / 5.0f ;
        if ( angle_ > 4.0f * motor::math::constants<float_t>::pi() ) angle_ = 0.0f ;

        float_t s = 5.0f * std::sin( angle_ ) ;

        struct the_data
        {
            motor::math::vec4f_t pos ;
            motor::math::vec4f_t col ;
            motor::math::vec4f_t frame_x ;
            motor::math::vec4f_t frame_y ;
            motor::math::vec4f_t frame_z ;

        };

        _cubes_data.data_buffer().resize( _max_objects ).
            update< the_data >( [&] ( the_data * array, size_t const ne )
        {
            typedef motor::concurrent::range_1d<size_t> range_t ;
            auto const & range = range_t( 0, std::min( size_t( _max_objects ), ne ) ) ;

            motor::concurrent::parallel_for<size_t>( range, [&] ( range_t const & r )
            {
                size_t const cubes_per_ring = 80 ;

                for ( size_t e = r.begin(); e < r.end(); ++e )
                {
                    float_t e0 = ((e) % cubes_per_ring) / float_t(cubes_per_ring) ;
                    float_t e1 = float_t( e / cubes_per_ring ) / (ne/cubes_per_ring) ;

                    size_t const cur_idx = (e / cubes_per_ring)+0 ;
                    size_t const nxt_idx = (e / cubes_per_ring)+1  ;


                    float_t const t0 = float_t( cur_idx ) / float_t ( ne/cubes_per_ring ) ;
                    float_t const t1 = float_t( nxt_idx ) / float_t ( ne/cubes_per_ring ) ;

                    auto const v0 = _worm_pos_spline( t0 ) ;
                    auto const v1 = _worm_pos_spline( t1 ) ;

                    auto const dir = ( v1 - v0 ).normalized() ;

                    auto const up = motor::math::vec3f_t(0.0f, 1.0f, 0.0f ) ;// off.normalized() ;
                    
                    auto const ortho_ = ( up - ( dir * dir.dot( up ) ) ).normalized() ;
                    //auto const right = dir.crossed( ortho ).normalized() ;

                    
                    motor::math::quat4f_t const axis( e0 * 2.0f * motor::math::constants<float_t>::pi(), dir ) ;
                    
                    //motor::math::m3d::trafof_t t ;
                    //t.rotate_by_angle_fl( motor::math::vec3f_t( e0 * 2.0f * motor::math::constants<float_t>::pi(), 0.0f, 0.0f ) ) ;
                    
                    auto const ortho = axis.to_matrix() * ortho_ ;
                    auto const right = dir.crossed( ortho ).normalized().negated() ;


                    #if 1
                    


                    
                    auto const base = v0 + ortho * 300.0f;
                    auto const pos = motor::math::vec4f_t( base, 20.0f ) ;
                    array[ e ].frame_x = motor::math::vec4f_t( right, 1.0f ) ;
                    array[ e ].frame_y = motor::math::vec4f_t( ortho, 1.0f ) ;
                    array[ e ].frame_z = motor::math::vec4f_t( dir, 1.0f ) ;

                    #else
                    float_t const v = float_t(e) / float_t(ne)  ;
                    float_t const x = 100.0f * v - 50.0f ;
                    float_t const y = 50.0f * std::sin( 2.0f * motor::math::constants<float_t>::pi() * v + s ) ;
                    float_t const z = 0.0f ;

                    motor::math::vec4f_t const pos( x, y, z, 30.0f ) ;
                    #endif

                    array[ e ].pos = pos ;
                    array[ e ].col = motor::math::vec4f_t ( 1.0f, 0.5f, 0.5f, 1.0f ) ;
                }
            } ) ;
        } ) ;
    }

    // draw worm path
    #if 1
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

        _cubes_debug_msl.for_each( [&] ( size_t const i, motor::graphics::variable_set_mtr_t vs )
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

        _cubes_final_msl.for_each( [&] ( size_t const i, motor::graphics::variable_set_mtr_t vs )
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
}

//*******************************************************************************
void_t scene_0::on_render_debug( bool_t const initial, motor::graphics::gen4::frontend_ptr_t fe ) noexcept
{
    if( initial )
    {
        fe->configure< motor::graphics::state_object_t>( &_debug_rs ) ;
        fe->configure<motor::graphics::geometry_object>( _dummy_geo ) ;
        fe->configure<motor::graphics::msl_object>( _dummy_debug_msl ) ;

        fe->configure<motor::graphics::array_object>( &_cubes_data ) ;
        fe->configure<motor::graphics::geometry_object>( &_cubes_geo ) ;
        fe->configure<motor::graphics::msl_object>( &_cubes_debug_msl ) ;
    }

    // render
    {
        fe->update( &_cubes_data ) ;

        fe->push( &_debug_rs ) ;
        
        // render cubes
        {
            motor::graphics::gen4::backend_t::render_detail_t detail ;
            detail.start = 0 ;
            detail.num_elems = _max_objects * 36 ;
            detail.varset = 0 ;
            fe->render( &_cubes_debug_msl, detail ) ;
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
void_t scene_0::on_render_final( bool_t const initial, motor::graphics::gen4::frontend_ptr_t fe ) noexcept
{
    if( initial )
    {
        //fe->configure< motor::graphics::state_object_t>( &_scene_final_rs ) ;
        fe->configure<motor::graphics::geometry_object>( _dummy_geo ) ;
        fe->configure<motor::graphics::msl_object>( _dummy_render_msl ) ;

        fe->configure<motor::graphics::array_object>( &_cubes_data ) ;
        fe->configure<motor::graphics::geometry_object>( &_cubes_geo ) ;
        fe->configure<motor::graphics::msl_object>( &_cubes_final_msl ) ;
    }

    fe->update( &_cubes_data ) ;

    // render scene
    {
        // render cubes
        {
            motor::graphics::gen4::backend_t::render_detail_t detail ;
            detail.start = 0 ;
            detail.num_elems = _max_objects * 36 ;
            detail.varset = 0 ;
            fe->render( &_cubes_final_msl, detail ) ;
        }

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