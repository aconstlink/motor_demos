
#include <motor/base/types.hpp>
#include <motor/std/vector>

#include <array>

namespace demos
{
    using namespace motor::core::types ;

    struct audio_analysis
    {
        motor::vector< float_t > captured_samples ;
        motor::vector< float_t > captured_frequencies ;

        motor::vector< float_t > captured_frequencies_avg ;
        motor::vector< float_t > captured_frequencies_var ;

        // based on idea of:
        // https://www.parallelcube.com/2018/03/30/beat-detection-algorithm/
        struct analysis
        {
        public: //

            int_t kick_start = 50 ;
            int_t kick_band = 70 ;

        public: // computation

            size_t kick_history_idx = size_t( -1 ) ;
            size_t midl_history_idx = size_t( -1 ) ;

            std::array< float_t, 43 > kick_history ;
            std::array< float_t, 43 > midl_history ;

            float_t kick_history_single = 0.0f ;
            float_t midl_history_single = 0.0f ;

            float_t kick_average = 0.0f ;
            float_t midl_average = 0.0f ;

            void_t write_kick( float_t const v ) noexcept ;

            void_t write_midl( float_t const v ) noexcept ;

            void_t recompute_average( void_t ) noexcept ;

            float_t compute_kick_variance( void_t ) const noexcept ;

            float_t compute_midl_variance( void_t ) const noexcept ;

        public: // for visualization 

            bool_t is_kick = false ;
            bool_t is_lowm = false ;

            float_t kick = 0.0f ;
            float_t midl = 0.0f ;
        };

        analysis asys ;
    };
}